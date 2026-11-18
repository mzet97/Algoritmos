---
layout: default
---

# Aula 32a — Kernel didático RISC-V: boot, paginação, traps, syscall e escalonamento

**Competências SDD:** OS, ARQ, MEM, CON e SEC. **Pré-requisitos:** aulas 10a (arquitetura e memória virtual), 11, 12, 15, 16a e o [piloto de C](MAPA_SDD.md). Você precisa saber ler assembly RISC-V básico, montar uma tabela de páginas Sv39 no papel e compilar com o cross-compiler da Fase 1 (`riscv64-linux-gnu-gcc`, QEMU 10.2.2 registrados no [CONTINUAR_AQUI](../CONTINUAR_AQUI.md)).

**Objetivo observável:** ao terminar, você deverá inicializar o kernel em `teoria/c/kernel/` no QEMU (`-machine virt -bios none -smp 2`), e demonstrar na serial, com marcadores verificados por teste automatizado: boot, segundo hart, interrupção de timer, syscall por `ecall`, falta de página sob demanda com reexecução da instrução e escalonamento preemptivo entre duas tarefas em modo-S.

**Motivação:** até aqui, "sistema operacional" foi observado de fora: `/proc`, syscalls de um processo, página medida pela 10a. Esta aula atravessa a fronteira: o programa **é** o modo privilegiado. Cada mecanismo que as aulas anteriores mediram pelo lado de fora — tabela de páginas, trap, escalonador — aparece aqui pelo lado de dentro, em ~500 linhas que cabem numa leitura.

## 1. Contrato antes do código

Ambiente e promessas (todas verificadas pelo `test_kernel.py`):

- **Plataforma:** QEMU virt, 2 harts, sem firmware (`-bios none`): o kernel nasce em modo-M e é dono de todas as traps. RAM em `0x80000000`, UART em `0x10000000`, CLINT em `0x2000000`, desligamento pelo test device em `0x100000` (`0x5555` = sucesso, `0x3333` = falha).
- **Marcadores:** `BOOT_OK`, `HART1_OK`, `TIMER_OK`, `SYSCALL_OK`, `FAULT_OK`, `SCHED_OK`, `DONE` — todos, nesta ordem de dependência, e o QEMU sai 0 pelo test device (não por timeout).
- **Paginação:** Sv39 com duas gigapages identidade (dispositivos e RAM) ligadas **antes** de descer para modo-S. O endereço `0x100000000` nasce sem mapa de propósito: a primeira leitura dispara a falta que o kernel trata mapeando, com `sfence.vma`, e reexecutando a instrução.
- **Fora do contrato:** sistema de arquivos, drivers além da UART por polling, proteção entre as duas tarefas (ambas enxergam o mesmo mapa), SMP de verdade (o hart 1 só anuncia e espera). É um kernel didático, e se apresentar como tal é parte do contrato.

## 2. As etapas, na ordem em que o hardware impõe

**Boot (modo-M).** O `_start` dá uma pilha por hart, zera a `.bss` (carregador não é contrato), instala `mtvec` e `mscratch` e chama `kmain`. O hart 1 espera um sinal em memória e imprime `HART1_OK`: multicore aqui é "o segundo núcleo acordou e falou", sem fingir escalonamento entre harts.

**PMP antes de tudo.** Sem OpenSBI, a PMP (Physical Memory Protection) nasce fechada e o modo-S não acessa **nem a RAM**: a primeira tentativa de descer morre com causa 1 (instruction access fault) **na instrução seguinte ao `mret`**, com `mepc` apontando para o próprio `mret` — diagnóstico que custa caro sem a tabela de causas na mão. A correção é de duas linhas: `pmpaddr0 = -1`, `pmpcfg0 = 0xF` (TOR até o topo, R/W/X). Firmware normalmente faz isso por você; sem firmware, a PMP é sua primeira configuração, antes até da paginação.

**Paginação Sv39.** A raiz tem duas entradas folha de 1 GiB: VA `0x0` → PA `0x0` (UART/CLINT, R/W) e VA `0x80000000` → PA `0x80000000` (RAM, R/W/X), com bits A/D ligados. `satp` recebe modo Sv39 + endereço da raiz, seguido de `sfence.vma`. Gigapage é a escolha didática: uma entrada por região, nenhum nível intermediário para depurar na primeira subida. O preço — granularidade de 1 GiB — está declarado.

**Descida e tarefas.** `mstatus.MPP = S`, `MPIE = 1`, e um `mret` artesanal (`lancar_tarefa`) restaura o frame inicial da tarefa A. As duas tarefas rodam em modo-S, nas suas pilhas, imprimindo `A` e `B` por MMIO na UART — que o mapa identidade de dispositivos alcança.

**Traps e syscall.** Toda trap cai em modo-M (nada é delegado): `trap_entry` troca `sp` com `mscratch` (pilha de trap própria; o `sp` da tarefa nunca é confiável), salva x1–x31 e `mepc` num `TrapFrame` e chama o tratador em C. `ecall` com `a7 = 42` é a syscall de teste: imprime `SYSCALL_OK` e avança `mepc` em 4. Esquecer o `+4` é o loop infinito clássico: a mesma `ecall` reexecuta para sempre.

**Falta sob demanda.** A tarefa A lê `0x100000000`: causa 13, `mtval` com o endereço. O tratador reconhece o endereço contratado, instala a gigapage apontando para a RAM, dá `sfence.vma`, imprime `FAULT_OK` e devolve o frame **sem tocar em `mepc`** — a instrução que faltou reexecuta e agora lê. Qualquer outra falta é `FALTA_INESPERADA` e desliga com falha: falta fora do contrato não é "tratada", é denunciada.

**Escalonador preemptivo.** O timer do CLINT (10 ms) interrompe as tarefas em modo-S. A cada tick, o tratador copia o frame da tarefa atual para a estrutura dela, alterna e devolve o frame da outra — quem restaura é o assembly, que não sabe (nem precisa saber) que a tarefa mudou. Após 10 trocas com todos os marcadores acesos, o próprio tratador imprime `SCHED_OK`/`DONE` e desliga. É round-robin de duas tarefas com quantum por timer: preemptivo de verdade, política mínima declarada.

## 3. Exemplo completo e executável

Fontes em `teoria/c/kernel/` ([kernel.c](kernel/kernel.c), `boot.S`, `trap.S`, `link.ld`, `Makefile` e o teste `test_kernel.py`).

```bash
make -C teoria/c/kernel
python3 teoria/c/kernel/test_kernel.py teoria/c/kernel/kernel.elf
```

Saída esperada:

```text
KERNEL_OK marcadores=BOOT_OK,HART1_OK,TIMER_OK,SYSCALL_OK,FAULT_OK,SCHED_OK,DONE
```

A serial crua (com `timeout 10 qemu-system-riscv64 -machine virt -cpu rv64 -smp 2 -bios none -kernel teoria/c/kernel/kernel.elf -nographic`) mostra os `A`/`B` intercalados entre os marcadores — a intercalação **é** a evidência do escalonador, não ruído. Rodar com `-smp 1` deve falhar por ausência de `HART1_OK`: o teste exige os dois harts.

### O que a memória faz aqui?

Três pilhas com donos distintos: a de boot/modo-M por hart (16 KiB), a de trap do hart 0 (16 KiB, via `mscratch`) e a de cada tarefa em modo-S (8 KiB). Confundir a de trap com a da tarefa é o bug estrutural deste kernel: `sp` ao entrar na trap pertence à tarefa interrompida, e salvar registradores nele escreveria na pilha de quem não deve saber. O `TrapFrame` de 264 bytes por tarefa é todo o "PCB" desta aula: registradores + `mepc`, copiado na troca.

## 4. Erros comuns e implicações defensivas

1. **Esquecer a PMP:** sem firmware, modo-S não acessa nada e a causa 1 aponta para o `mret`. O defeito é de configuração de hardware, não do código da tarefa.
2. **`ecall` sem avançar `mepc`:** syscall que volta para a mesma instrução é um loop infinito silencioso. Toda syscall que "retorna" avança; só falta de página reexecuta.
3. **Salvar a trap na pilha da tarefa:** `sp` interrompido é entrada hostil. `mscratch` com pilha de trap própria, sempre.
4. **Mapear na falta sem `sfence.vma`:** a TLB pode guardar a tradução inválida; a instrução reexecutada falta de novo — com a página já mapeada, o laço se disfarça de "a falta não resolve".
5. **Tratar qualquer falta como a falta esperada:** endereço fora do contrato é bug da tarefa ou ataque ao mapa; mapear o que pedirem é transformar o tratador em cúmplice. Aqui, desliga com falha e imprime o endereço.
6. **UART compartilhada sem dono:** tarefas e tratador escrevem na mesma serial; marcador pode intercalar no meio de outro. Nos testes, marcadores do tratador saem com as tarefas paradas (dentro da trap) — quem imprime concorrente com tarefa (o hart 1) faz isso uma vez, antes das tarefas nascerem.

## 5. Ponte com as demais trilhas

C++ (CPP-12) e Rust (R8) repetem este percurso nas suas trilhas: boot, trap, páginas, syscall, escalonador, comparação x86-64. Go e Python, pelo contrato da masterclass, **não** fingem kernel: estudam o mesmo SO por ferramentas de espaço de usuário. E a comparação x86-64 deste livro fica como etapa seguinte declarada: o componente portável é o modelo (frame, fila, mapa), não o assembly — `mtvec`, CLINT e PMP são RISC-V e terão outros nomes lá.

## 6. Exercícios obrigatórios

1. Adicione a syscall `a7 = 1` ("imprime o caractere em `a0`") e mova a saída das tarefas para ela, retirando o MMIO direto da UART em modo-S. O que muda no modelo de confiança?
2. Mude o quantum para 1 tick e depois para 50 ticks e conte os `A`/`B` por segundo em cada caso. Onde o custo da troca aparece?
3. Faça a tarefa B ler o endereço `0x200000000` (fora do contrato) e mostre o `FALTA_INESPERADA` com causa e `mtval`. Por que desligar é a resposta certa aqui?
4. Troque a gigapage de RAM por páginas de 4 KiB só para a pilha da tarefa B (níveis L1/L0) e meça quantas entradas a tabela passa a ter. O que a granularidade compra e o que ela custa?

Use [pistas graduais](exercicios/32a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/32a_solucoes.md) depois de registrar sua própria explicação.

## 7. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- explicar a ordem boot → PMP → paginação → descida, e diagnosticar a causa 1 no `mret`;
- desenhar o caminho de uma trap: `mscratch`, `TrapFrame`, tratador, restauração (possivelmente de outra tarefa);
- distinguir as três respostas possíveis a uma trap: avançar `mepc` (syscall), não tocar (falta tratada), desligar (fora do contrato);
- demonstrar os sete marcadores com o teste automatizado e explicar a intercalação `A`/`B`;
- declarar o que o kernel **não** faz: SMP real, isolamento entre tarefas, FS, drivers;
- reconstruir do zero (`make clean && make && test`) em clone limpo.

**Próxima etapa do livro:** comparação x86-64 (mesmo modelo, outra arquitetura) e o gate do volume de C. O kernel é a última peça de C4/C8; o que falta para o gate é a revisão integral das aulas 01–32 contra este padrão.
