---
layout: default
---

# Aula 28a — Kernel RISC-V em Rust `no_std`: o que o `core` compra e o que ele cobra

**Competências SDD:** OS, MEM, ARQ, SEC, CON. **Pré-requisitos:** aulas 15–16 (ownership e `unsafe`/FFI auditáveis), [16a](16a_maquina_e_so.md), 27–28 e o percurso da [aula 32a de C](../c/32a_kernel_riscv.md) como referência do modelo. Toolchain: `rustc` 1.99 com o target `riscv64gc-unknown-none-elf` (`rustup target add riscv64gc-unknown-none-elf`) e QEMU 10.2.2.

**Objetivo observável:** ao terminar, você deverá compilar o kernel de `teoria/rust/kernel/` só com `core` (sem `std`, sem alocador), inicializá-lo no QEMU com os mesmos sete marcadores das trilhas C/C++ e explicar cada ponto onde o Rust exigiu uma prova ou uma declaração que C não exigia — e o ponto onde ele não ajudou em nada.

**Motivação:** o mapa (R8) pedia um kernel Rust de verdade, e a pergunta honesta não é "dá para escrever?", é "o que muda quando a linguagem leva ownership e ausência de runtime a sério num lugar onde tudo é `static mut` e MMIO?" A resposta curta desta aula: muda onde dói menos (fronteiras tipadas, `Result` substituído por códigos de trap) e não muda onde dói mais (a tabela de páginas continua sendo aritmética crua dentro de `unsafe`).

## 1. Contrato antes do código

- **Build:** `rustc --edition 2024 --target riscv64gc-unknown-none-elf -C panic=abort --crate-type staticlib`, link com o gcc cruzado (`-mabi=lp64d`, o ABI do target Rust — misturar com `lp64` é o primeiro erro de link). Sem `alloc`: não há heap até alguém escrevê-lo, exatamente como nas aulas 33 de C++.
- **Marcadores idênticos:** `BOOT_OK HART1_OK TIMER_OK SYSCALL_OK FAULT_OK SCHED_OK DONE` pelo mesmo `test_kernel.py`. O teste é do modelo; três linguagens passam pelo mesmo.
- **Pânico é desligamento:** `#[panic_handler]` imprime `PANIC` e desliga com falha. Num kernel sem runtime, pânico não é exceção recuperável; é o último relatório. `panic=abort` impede a maquinaria de unwind de ser linkada à força.
- **Fora do contrato:** alocador, drivers além da UART, SMP real e isolamento entre tarefas — idêntico às trilhas irmãs, declarado de novo porque linguagem nova não herda honestidade automaticamente.

## 2. Onde o Rust cobrou — três recibos reais do build

1. **`static mut` na edição 2024:** referenciar `static mut` é erro; o kernel inteiro é estado global mutável (tarefas, ticks, tabela). A forma aceita é `addr_of_mut!` + ponteiro cru dentro de `unsafe`, ou seja: a linguagem não eliminou o estado global, ela obrigou a escrever, em cada acesso, que você sabe que ele é global e compartilhado com uma trap assíncrona. É burocracia com valor: cada `unsafe` é um ponto de auditoria contável (`grep` acha todos; em C, qualquer linha pode ser o problema).
2. **Alinhamento da tabela raiz:** a primeira subida morreu com causa 12 (instruction page fault) porque um `[u64; 512]` comum tem alinhamento 8, e a raiz Sv39 precisa de 4096. A correção é um tipo `#[repr(C, align(4096))]`. Em C o erro análogo é esquecer o atributo; aqui o **tipo** carrega o requisito, e depois de declarado, nenhum uso futuro consegue desalinhar sem mudar o tipo.
3. **`unsafe extern` e `#[unsafe(no_mangle)]`:** a edição 2024 exige declarar que a fronteira com o assembly é insegura **na declaração dela**. `kmain` e `trap_handler` são `extern "C"` porque o `trap.S` os chama pelo nome — o compilador obriga a escrever que esse nome é um contrato manual.

E onde ele não ajudou: `mapear_giga` continua sendo conta de PPN dentro de `unsafe`, o `TrapFrame` continua sendo um layout `#[repr(C)]` que precisa casar com o assembly campo a campo, e um erro de índice no frame compila feliz nas três linguagens. Rust concentra o perigo em blocos contáveis; não o aposenta.

## 3. Exemplo completo e executável

```bash
make -C teoria/rust/kernel
python3 teoria/rust/kernel/test_kernel.py teoria/rust/kernel/kernel.elf
# KERNEL_OK marcadores=BOOT_OK,HART1_OK,TIMER_OK,SYSCALL_OK,FAULT_OK,SCHED_OK,DONE
```

Comparação que a aula exige fazer: contar os `unsafe` deste `main.rs` e listar o que cada um protege (MMIO, CSR, tabela, frame, statics). Se algum `unsafe` não tiver uma frase de justificativa defensável, ele é um defeito de auditoria mesmo com o kernel verde. O segundo exercício de leitura é o `panic_handler`: num sistema com `std`, `panic!` é diagnóstico; aqui, é a última instrução antes do desligamento — e qualquer `unwrap`/`expect`/indexação no caminho da trap é um desligamento esperando a entrada certa, o que nos leva à regra prática: no kernel, indexação só com índices provados ou `get_unchecked` **auditado e comentado** (este kernel usa índices constantes sobre arrays de tamanho fixo, e é por isso que eles são defensáveis).

## 4. Erros comuns e implicações defensivas

1. **Linkar com o ABI errado:** `lp64` contra `lp64d` falha na junção com "double-float × soft-float". ABI é contrato de link, e kernel é onde ninguém abstrai isso por você.
2. **`static mut` "só desta vez":** na edição 2024 ela nem compila como referência; contornar com um `Mutex` de mentira em núcleo único com traps assíncronas troca um problema visível por um deadlock invisível.
3. **Esquecer `panic=abort`:** sem ele, o linker procura a maquinaria de unwind que não existe em `no_std` — o erro aparece longe da causa, como na aula 33 de C++ com exceções.
4. **Achar que `unsafe` contado é `unsafe` auditado:** contar é o começo. Cada bloco precisa da frase do que o torna seguro (alinhamento garantido pelo tipo, índice constante, MMIO com endereço do manual).
5. **Tratar causa 12 como "bug da tarefa":** a primeira instrução em modo-S faltando é, quase sempre, tabela ou PMP — o diagnóstico da 32a de C vale aqui sem mudar uma palavra, porque o hardware é o mesmo.

## 5. Exercícios obrigatórios

1. Troque uma indexação constante do kernel por uma indexação com índice vindo de um registrador de trap e mostre o caminho até o `PANIC` (bounds check) quando o índice sai do intervalo. Por que no caminho da trap isso é pior do que retornar erro?
2. Envolva o acesso à UART num tipo `Uart` com método `putc` seguro por fora (`unsafe` só dentro) e conte quantos `unsafe` somem do resto do arquivo sem nenhum sumir do total real de risco.
3. Remova o `align(4096)` e reproduza a causa 12; depois restaure. Escreva o teste que pegaria esse defeito sem boot (dica: `debug_assert` sobre o endereço — onde ele roda e onde ele não roda?).
4. Liste todos os `unsafe` do `main.rs` com uma frase de justificativa cada. Qual deles você eliminaria primeiro com um tipo melhor, e qual é irredutível?

Use [pistas graduais](exercicios/28a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/28a_solucoes.md) depois de registrar sua própria explicação.

## 6. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- montar o build `no_std` explicando target, ABI e `panic=abort`;
- justificar cada `unsafe` com uma frase, e apontar os irredutíveis;
- diagnosticar causa 12 (alinhamento/PMP) sem culpar a tarefa;
- reproduzir `KERNEL_OK` em clone limpo e diff comportamental zero contra C/C++;
- declarar as lacunas (alocador, isolamento, SMP) sem atribuí-las à linguagem.

**Próxima aula:** [28b](28b_x86_64_comparativo.md) — o comparativo x86-64 desta trilha, fechando o R8.
