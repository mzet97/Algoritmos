/* Componente comparativo x86-64: Multiboot1 -> modo protegido -> tabelas
   de 4 niveis (pag. de 2 MiB, identidade 1 GiB) -> PAE + LME -> modo longo
   -> GDT64 -> IDT com handler de int3 -> serial -> isa-debug-exit.
   O objeto e ELF32 (exigencia do loader Multiboot do QEMU); por isso tudo
   que o codigo de 64 bits alcanca (mensagens, IDT, idt_ptr) mora na MESMA
   secao do codigo: as referencias RIP-relativas viram deslocamentos
   locais, sem relocacao de 64 bits que o ELF32 nao representa. */
    .set MB_MAGIC, 0x1BADB002
    .section .multiboot, "a"
    .align 4
    .long MB_MAGIC
    .long 0
    .long -(MB_MAGIC)

    .section .bss, "aw", @nobits
    .align 4096
pml4:   .skip 4096
pdpt:   .skip 4096
pd:     .skip 4096
    .align 16
pilha:  .skip 16384
pilha_topo:

    .section .rodata
gdt:
    .quad 0                    /* nulo */
    .quad 0x00AF9A000000FFFF   /* codigo 64: presente, executavel, L=1 */
    .quad 0x00CF92000000FFFF   /* dados */
gdt_ptr:
    .word gdt_ptr - gdt - 1
    .long gdt

    .section .text
    .code32
    .globl _start
_start:
    mov $pilha_topo, %esp
    /* PD: 512 entradas de 2 MiB, identidade, presente+rw+PS (0x83) */
    mov $pd, %edi
    mov $0x83, %eax
    mov $512, %ecx
1:  mov %eax, (%edi)
    add $0x200000, %eax
    add $8, %edi
    loop 1b
    /* PDPT[0] -> PD, PML4[0] -> PDPT (presente+rw = 0x3) */
    mov $pd, %eax
    or $0x3, %eax
    mov %eax, pdpt
    mov $pdpt, %eax
    or $0x3, %eax
    mov %eax, pml4
    mov $pml4, %eax
    mov %eax, %cr3
    /* PAE */
    mov %cr4, %eax
    or $(1 << 5), %eax
    mov %eax, %cr4
    /* EFER.LME */
    mov $0xC0000080, %ecx
    rdmsr
    or $(1 << 8), %eax
    wrmsr
    /* paging (CR0.PG) -> modo de compatibilidade */
    mov %cr0, %eax
    or $(1 << 31), %eax
    mov %eax, %cr0
    lgdt gdt_ptr
    ljmp $0x08, $longo

    .code64
longo:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    mov $0x200000, %rsp        /* pilha em RAM identidade (endereco fixo) */
    /* serial COM1 115200 8N1 */
    mov $0x3F9, %dx
    xor %al, %al
    out %al, %dx
    mov $0x3FB, %dx
    mov $0x80, %al
    out %al, %dx
    mov $0x3F8, %dx
    mov $0x01, %al
    out %al, %dx
    mov $0x3F9, %dx
    xor %al, %al
    out %al, %dx
    mov $0x3FB, %dx
    mov $0x03, %al
    out %al, %dx

    lea msg_boot(%rip), %rsi
    call imprime

    /* IDT[3] (int3) -> trap3: seletor 0x08, tipo 0x8E */
    lea idt(%rip), %rdi
    lea trap3(%rip), %rax
    mov %ax, 3 * 16(%rdi)
    movw $0x08, 3 * 16 + 2(%rdi)
    movb $0, 3 * 16 + 4(%rdi)
    movb $0x8E, 3 * 16 + 5(%rdi)
    shr $16, %rax
    mov %ax, 3 * 16 + 6(%rdi)
    shr $16, %rax
    mov %eax, 3 * 16 + 8(%rdi)
    lidt idt_ptr(%rip)

    int3

    lea msg_done(%rip), %rsi
    call imprime
    /* isa-debug-exit: QEMU sai com (valor << 1) | 1 = 33 */
    mov $0xF4, %dx
    mov $0x10, %eax
    out %eax, %dx
1:  hlt
    jmp 1b

trap3:
    push %rax
    push %rdx
    push %rsi
    lea msg_trap(%rip), %rsi
    call imprime
    pop %rsi
    pop %rdx
    pop %rax
    iretq

/* imprime string terminada em 0 apontada por RSI na COM1 (poll THRE). */
imprime:
    push %rax
    push %rdx
    push %rsi
1:  mov $0x3FD, %dx
    in %dx, %al
    test $0x20, %al
    jz 1b
    mov (%rsi), %al
    test %al, %al
    jz 2f
    mov $0x3F8, %dx
    out %al, %dx
    inc %rsi
    jmp 1b
2:  pop %rsi
    pop %rdx
    pop %rax
    ret

/* Dados alcancados em 64 bits: mesma secao do codigo (ver cabecalho). */
    .align 16
idt_ptr:
    .word 256 * 16 - 1
    .long idt          /* base de 8 bytes do LIDT: endereco < 4 GiB, */
    .long 0            /* metade alta escrita a mao (ELF32 sem reloc 64) */
    .align 16
idt:
    .skip 256 * 16
msg_boot:  .asciz "X86_BOOT_OK\n"
msg_trap:  .asciz "X86_TRAP_OK\n"
msg_done:  .asciz "X86_DONE\n"
