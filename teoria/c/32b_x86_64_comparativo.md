---
layout: default
---

# Aula 32b — O mesmo modelo em x86-64: Multiboot, modo longo e IDT

**Competências SDD:** OS, ARQ e SEC. **Pré-requisitos:** aula [32a](32a_kernel_riscv.md) concluída e verificada no QEMU. Você precisa ter presente o modelo da 32a (frame de trap, tabela de páginas, syscall, escalonador) para comparar mecanismo a mecanismo — esta aula não repete o kernel inteiro, executa a **transição** na outra arquitetura.

**Objetivo observável:** ao terminar, você deverá inicializar o componente de `teoria/c/kernel-x86/` pelo loader Multiboot do QEMU, atravessar protegido → paginação de 4 níveis → PAE/LME → modo longo, instalar uma IDT com handler de `int3`, imprimir os três marcadores na serial e desligar pelo `isa-debug-exit` — e explicar, com a tabela da seção 2, o que é modelo portável e o que era nome RISC-V para ele.

**Motivação:** a 32a terminou com uma promessa: "o que porta é o modelo, não o assembly". Promessa sem execução é propaganda. Este componente — um único arquivo assembly, sem pretensão de kernel completo — executa a travessia equivalente em x86-64 e marca onde a analogia quebra: aqui não há modo-M dono de tudo, a CPU nasce em modo real/protegido pelas mãos de um loader, e até o formato do binário vira decisão de arquitetura.

## 1. Contrato antes do código

- **Plataforma:** QEMU `q35`, 64 MiB, `-kernel` com Multiboot1 (o loader entra em modo protegido de 32 bits, paging desligado, `EAX = 0x2BADB002`). Desligamento pelo `isa-debug-exit` (porta `0xF4`, valor `0x10`): o QEMU sai com `(0x10 << 1) | 1 = 33`, que o teste contrata como **sucesso** — documentado porque 33 como "sucesso" sem contexto é indistinguível de erro.
- **Marcadores:** `X86_BOOT_OK` (modo longo ativo, paginação ligada), `X86_TRAP_OK` (handler da IDT executou e `iretq` voltou), `X86_DONE`.
- **Fora do contrato:** kernel x86 completo. Syscall (`syscall`/`sysenter`), SMP/APIC, IRQs de timer e tarefas são a etapa seguinte declarada nos exercícios — o componente prova a travessia e a trap, que são exatamente os pontos onde as arquiteturas divergem.

## 2. O mapa de correspondências — e onde ele quebra

| Modelo (32a) | RISC-V | x86-64 aqui | Porta? |
|---|---|---|---|
| Entrada | `_start` em modo-M, todos os harts | Multiboot entrega protegido 32 bits, um núcleo | Conceito sim; quem acorda os outros núcleos é APIC INIT/SIPI lá |
| Endereço de trap | `mtvec` | IDT + `lidt`, vetor 3 (`int3`) | Sim: tabela de handlers indexada |
| Salvamento | `TrapFrame` manual em pilha de trap (`mscratch`) | CPU empilha erro/RIP/CS/RFLAGS; o resto é manual igual | O trabalho extra é o mesmo |
| Paginação | Sv39, 3 níveis, gigapage 1 GiB | 4 níveis, páginas de 2 MiB (512 entradas = 1 GiB identidade) | Sim: walk multinível com folha grande |
| Proteção pré-S | PMP aberta na mão (sem firmware) | Não há equivalente direto; anéis e o próprio paging fazem o papel | **Não**: PMP é resposta a um problema que x86 resolve de outro jeito |
| Syscall | `ecall` (causa 9) | `syscall`/`int 0x80` (fora deste componente) | O mecanismo muda; o contrato (número em registrador, `mepc`+4 ≈ RIP salvo) não |
| Timer | CLINT `mtimecmp` | PIT/APIC timer + IRQ na IDT | Endereço e dono mudam; quantum é o mesmo |
| Desligar | test device `0x100000 = 0x5555` | `isa-debug-exit` `0xF4` | Convenção do emulador, nas duas |

A linha da PMP é a lição da tabela: nem todo mecanismo tem correspondente. Quem porta procurando "onde fica a PMP do x86" não entendeu nem a PMP (ela existe porque RISC-V separa proteção física de tradução) nem o x86 (onde tradução e anéis nasceram juntos).

## 3. Exemplo completo e executável

Fontes em [teoria/c/kernel-x86/](kernel-x86/): `boot.S`, `link.ld`, `Makefile`, `test_kernel_x86.py`.

```bash
make -C teoria/c/kernel-x86
python3 teoria/c/kernel-x86/test_kernel_x86.py teoria/c/kernel-x86/kernel-x86.elf
# KERNEL_X86_OK marcadores=X86_BOOT_OK,X86_TRAP_OK,X86_DONE
```

A sequência do `boot.S` (tudo assembly AT&T, a sintaxe que o GAS emite e lê):

1. **Tabelas antes de tudo:** PD com 512 entradas de 2 MiB identidade (presente + R/W + PS), PDPT e PML4 encadeados — montadas em 32 bits, antes de qualquer paging.
2. **A travessia:** `CR3` ← PML4, `CR4.PAE`, `EFER.LME` via `rdmsr`/`wrmsr` no MSR `0xC0000080`, `CR0.PG`. A CPU entra em modo de compatibilidade; o `ljmp $0x08, $longo` para o segmento de código com bit L liga o modo longo de verdade. Pular uma etapa qualquer é triple fault sem mensagem — a ordem é contrato de hardware, não estilo.
3. **Serial e IDT:** COM1 a 115200 8N1 por polling de THRE; entrada 3 da IDT montada campo a campo (offset em três pedaços, seletor `0x08`, tipo `0x8E`); `int3` dispara o handler, que imprime `X86_TRAP_OK` e retorna por `iretq` — o RIP salvo já aponta para depois do `int3`, ao contrário do `ecall`, ninguém soma 4 aqui.

### O detalhe que custou o build: ELF32 com código de 64 bits

O loader Multiboot do QEMU recusa ELF64 ("Cannot load x86-64 image, give a 32bit one"), e o ELF32 não representa relocações de 64 bits. A saída usada aqui: **tudo que o código de 64 bits alcança** (mensagens, IDT, `idt_ptr`) **mora na mesma seção do código** — referências RIP-relativas viram deslocamentos locais resolvidos pelo montador, sem relocação. A base de 8 bytes do `lidt` é escrita como `.long idt` + `.long 0`, porque o endereço está abaixo de 4 GiB. É um truque honesto e declarado no cabeçalho do fonte: formato de binário também é arquitetura.

## 4. Erros comuns e implicações defensivas

1. **Ordem da travessia trocada:** ligar `CR0.PG` antes de `EFER.LME`, ou saltar antes de `lgdt`, é triple fault mudo. Em x86, depurar sem serial precoce é depurar no escuro: a primeira instrução útil deste componente prepara a saída.
2. **Segmento de código sem bit L:** o far jump entra em compatibilidade e o código de 64 bits decodifica como 32 — sintomas que imitam bug de lógica. O descritor `0x00AF9A...` carrega o L; confira o nibble antes de culpar o código.
3. **IDT montada com offset inteiro:** o campo offset é fatiado (0–15, 16–31, 32–63) por desenho de 16 bits que cresceu; montar de uma vez corrompe o tipo/seletor vizinho.
4. **`iretq` com pilha desbalanceada:** handler que empilha registradores e esquece de desempilhar retorna para o lixo empilhado como se fosse RIP. Salvar tudo e restaurar tudo, na ordem inversa, é o `TrapFrame` desta arquitetura.
5. **Confundir saída 33 com erro:** código de saída de emulador é convenção do dispositivo de desligamento. Sem o contrato escrito no teste, o próximo leitor "corrige" o sucesso para 0 e quebra a verificação.
6. **Achar que Multiboot é o boot real:** firmware de verdade (BIOS/UEFI) entrega outro estado. Multiboot é o laboratório; a aula diz isso em vez de deixar o aluno descobrir num hardware.

## 5. Ponte com a 32a e com as demais trilhas

O kernel RISC-V continua sendo **o** projeto de C (C8): completo, com tarefas e escalonador. Este componente é a etapa comparativa que o mapa pedia: mesma serial, mesmo "handler imprime e volta", mesma folha grande cobrindo 1 GiB — e PMP, CLINT e `mscratch` ficaram para trás como nomes locais de ideias que aqui se chamam anéis, APIC e pilha da IDT (com IST, quando configurado). C++ (CPP-13) e Rust (R8) fazem a comparação equivalente nas suas trilhas; a tabela da seção 2 é reutilizável nas três, trocando a coluna do meio.

## 6. Exercícios obrigatórios

1. Adicione a entrada 0 da IDT (divide-error) com o mesmo handler e dispare `div` por zero. O que a CPU empilha de diferente em relação ao `int3`, e o que o handler precisa fazer para "voltar" de uma instrução que não pode reexecutar?
2. Troque as páginas de 2 MiB por páginas de 4 KiB para o primeiro MiB (adicione o nível PT) e conte as tabelas. Onde a granularidade muda o que uma falta de página consegue proteger?
3. Implemente a syscall mínima: entrada `0x80` na IDT, número em `RAX`, e mova a impressão das mensagens para ela. Compare com o `ecall` da 32a, campo a campo.
4. Escreva o parágrafo de boot real: o que BIOS/UEFI entregaria de diferente do Multiboot (modo, tabelas, mapa de memória) e o que deste componente sobreviveria intacto.

Use [pistas graduais](exercicios/32b_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/32b_solucoes.md) depois de registrar sua própria explicação.

## 7. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- ordenar a travessia protegido → 4 níveis → PAE → LME → PG → far jump, e dizer o sintoma de cada troca;
- montar uma entrada de IDT campo a campo e explicar o que a CPU empilha numa exceção e num `int3`;
- preencher a tabela de correspondências incluindo a linha que **não** porta (PMP);
- explicar por que o binário é ELF32 com dados do código de 64 bits na mesma seção;
- reproduzir os três marcadores pelo teste automatizado em clone limpo;
- declarar o que falta para um kernel x86 (syscall, APIC/timer, SMP) sem inflar o componente.

**Próxima etapa do livro:** revisão integral das aulas 01–32 contra o padrão das pontes e o gate do volume de C. Com kernel e comparativo executados, C4 e C8 fecham o conteúdo novo; o que resta é revisão, não invenção.
