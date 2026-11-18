---
layout: default
---

# Aula 28b — Comparativo x86-64 da trilha Rust: fechar o R8

**Competências SDD:** OS, ARQ. **Pré-requisitos:** aula [28a](28a_kernel_riscv.md) e a leitura da [32b de C](../c/32b_x86_64_comparativo.md). O componente executado é o mesmo (`teoria/rust/kernel-x86/`, rebuild do componente da trilha de C) — a aula é a comparação lida com os olhos de quem acabou de escrever o kernel em Rust.

**Objetivo observável:** executar `KERNEL_X86_OK`, reproduzir a tabela de correspondências da 32b de C de memória, e responder onde o sistema de tipos de Rust ajudaria **neste componente** se ele fosse escrito em Rust (e onde o assembly continuaria assembly).

**Contrato:** `make -C teoria/rust/kernel-x86 && python3 teoria/rust/kernel-x86/test_kernel_x86.py teoria/rust/kernel-x86/kernel-x86.elf` → `KERNEL_X86_OK`. Fora do contrato: kernel x86 em Rust (a travessia em assembly já prova o mecanismo; reescrevê-la em Rust é o exercício 2).

## A comparação, com a pergunta nova

A tabela da 32b de C continua valendo linha a linha (Multiboot contra `_start` em modo-M, IDT contra `mtvec`, 4 níveis contra Sv39, PMP sem correspondente, CLINT contra APIC). A pergunta que a trilha Rust adiciona: **o que mudaria se o lado x86 também fosse Rust `no_std`?**

- A travessia (tabelas, PAE, LME, far jump) continuaria assembly puro: são instruções privilegiadas em sequência contratual, sem abstração que as melhore. O arquivo `.S` encolheria pouco ou nada.
- A IDT ganharia um tipo: entradas como struct `#[repr(C, packed)]` com construtor que fatia o offset — o erro de montar o offset de uma vez (seção 4 da 32b) viraria erro de tipo em vez de corrupção do campo vizinho.
- O handler ganharia a mesma contabilidade de `unsafe` da 28a: cada acesso a registrador de controle dentro de `asm!` declarado, cada endereço MMIO encapsulado.
- E nada disso mudaria o boot: Multiboot continuaria exigindo ELF32 com entrada em 32 bits, e o kernel Rust de 64 bits continuaria precisando de um stub — o formato do binário, como a 32b ensinou, também é arquitetura.

### Tradução trabalhada: a entrada da IDT como tipo (o que o exercício 1 cobra)

Uma entrada de IDT de 16 bytes guarda o endereço do handler fatiado em três campos (`offset_0_15`, `offset_16_31`, `offset_32_63`) mais seletor, tipo e DPL. O bug clássico da seção 4 da 32b — montar o offset de uma vez e corromper o campo vizinho — acontece porque assembly não distingue "endereço" de "três fatias": tudo é inteiro, e inteiro aceita qualquer fatiamento, inclusive errado. O tipo Rust `EntradaIdt::nova(handler: usize)` que o exercício pede carrega a distinção no construtor: ele recebe o endereço inteiro (o que o programador tem) e fatia (o que a tabela exige), com máscaras e deslocamentos num só lugar revisável — e `#[repr(C, packed)]` garante que o layout em memória é o da tabela, não o que o compilador preferir. O que o tipo NÃO resolve: escolher o handler certo, o DPL certo e o IST certo — decisão continua sendo do programador, e `unsafe` continua marcando onde a máquina é tocada. Tipo elimina classe de bug de montagem; não elimina decisão de arquitetura. Quem fizer o exercício vai notar que o teste "campos fatiados contra os valores do assembly" é comparação contra oráculo independente — o mesmo padrão da BST da 12b, agora aplicado a hardware.

## Exercícios obrigatórios

1. Reescreva a montagem da entrada da IDT como um tipo Rust `EntradaIdt::nova(handler: usize)` num programa de usuário comum (sem boot) e teste os campos fatiados contra os valores que o assembly grava.
2. Desenhe o stub Multiboot mínimo para um kernel Rust x86-64: o que ele faz em assembly antes de chamar `kmain` Rust, e onde o `#[unsafe(no_mangle)]` aparece.
3. Pegue a linha "pilha de trap" da tabela e escreva as duas frases que a tornam precisa nos dois mundos (IST configurado ou não; `mscratch` sempre).
4. Liste o que do R8 continua aberto depois desta aula se o critério for "kernel x86 completo em Rust" — e por que o mapa aceita o componente como etapa comparativa.

Use [pistas graduais](exercicios/28b_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/28b_solucoes.md) depois de registrar sua própria explicação.

## Critério de aprovação e checklist

- Os dois testes (`KERNEL_OK` RISC-V e `KERNEL_X86_OK`) verdes na mesma sessão, em clone limpo.
- A tabela de correspondências reproduzida de memória, com a linha sem correspondente marcada.
- O exercício 1 compilando como programa de usuário com testes de campo.
- A fronteira "componente comparativo" × "kernel x86 completo" declarada sem inflar o entregue.

**Fecho do R8:** kernel RISC-V em Rust executado e comparativo x86 executado. O que resta no livro de Rust é a revisão das aulas originais e os projetos 30–32 executados em ambiente limpo.
