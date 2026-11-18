---
layout: default
---

# Solucoes comentadas da aula 40

## 1. Syscall 0x80 de impressao

Entrada 0x80 na IDT apontando para um handler que le RAX: 1 = imprimir o byte em RDI pela rotina de serial existente; qualquer outro numero, ignorar com retorno. O texto sai identico ao do RISC-V porque a rotina de serial e a mesma ideia (poll de THRE na COM1 contra UART do virt). A diferenca que importa: o retorno e iretq com o RIP que a CPU empilhou; nao existe mepc para avancar — e tambem nao existe o bug de esquecer o +4.

## 2. A linha fragil

Aceita-se qualquer linha bem defendida; a defesa esperada cita comportamento: por exemplo, a pilha de trap — no RISC-V ela e 100% manual (mscratch), no x86 a CPU troca para a pilha do TSS/IST apenas se configurada, e neste componente (anel 0 para anel 0) ela nem troca. A correspondencia "mscratch = IST" e parcial e so vale com IST configurado; sem isso, a linha certa e "sem correspondente neste componente".

## 3. Acordar o segundo nucleo

x86 nasce com um nucleo acordado (BSP) e os demais (APs) parados apos o reset. O BSP acorda cada AP com a sequencia INIT IPI + Startup IPI via APIC local, apontando um trampolim em modo real abaixo de 1 MiB, que sobe por protegido ate o modo longo. No RISC-V do QEMU, todos os harts executam _start e a coordenacao e por memoria (o sinal liberar_harts da aula 33). Desenhos opostos: um acorda todos e segura, o outro acorda um e chama.

## 4. O que a CPU faz por voce

RIP, CS e RFLAGS sao empilhados pela CPU em qualquer interrupcao/excecao, e algumas excecoes empilham codigo de erro. No RISC-V, mepc/mcause/mtval sao CSRs que o software le e salva se quiser. O preco da ajuda x86: formato de pilha que varia por tipo de excecao (com/sem codigo de erro) e que o handler precisa conhecer vetor a vetor; o preco da liberdade RISC-V: todo o salvamento e manual, inclusive o que seria obvio.
