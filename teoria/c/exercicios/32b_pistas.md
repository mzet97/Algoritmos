---
layout: default
---

# Pistas da aula 32b

1. Entrada 0 segue o mesmo molde da entrada 3, trocando o índice e o handler. A diferença está no que a CPU empilha: exceção com código de erro empilha um `uint64` a mais que o `int3`. Para "voltar" de um `div` por zero que não pode reexecutar, o handler precisa avançar o RIP salvo para depois da instrução divisora — ou declarar que não volta e desligar, o que também é uma resposta, se documentada.
2. Páginas de 4 KiB pedem um quarto nível (PT) e perdem o bit PS nos níveis acima. Conte tabelas para 1 MiB: PML4 e PDPT já existem; o PD vira tabela de ponteiros e cada PT cobre 2 MiB — para 1 MiB, um PT basta.
3. A entrada `0x80` é uma entrada de IDT como outra qualquer: o número da syscall viaja em `RAX` (aqui) como o 42 viajava em `a7` no RISC-V. O que muda é quem salva o endereço de retorno: no x86 a própria instrução `int` empilha RIP/CS/RFLAGS; no `ecall`, o kernel lê `mepc` e soma 4.
4. Para o parágrafo: liste o que o Multiboot entrega pronto (protegido, GDT provisória, sem paging) contra o que BIOS legada (modo real 16 bits) e UEFI (modo longo já ativo, serviços próprios) entregariam. O que sobrevive intacto é tudo depois da travessia: serial, IDT e o modelo de handler.
