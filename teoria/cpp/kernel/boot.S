/* Boot RISC-V (QEMU virt, -bios none): todos os harts comecam em _start
   em modo-M. Hart 0 segue para kmain; os demais esperam o sinal de
   liberacao em memoria e anunciam HART1_OK (prova multicore). */
    .section .text.start
    .globl _start
_start:
    /* sp por hart: pilha_base + (hartid + 1) * 16 KiB */
    csrr t0, mhartid
    la   sp, _pilhas
    li   t1, 16384
    addi t2, t0, 1
    mul  t1, t1, t2
    add  sp, sp, t1

    bnez t0, hart_secundario

    /* zera a .bss (o carregador nao e contrato) */
    la   t0, _bss_ini
    la   t1, _bss_fim
1:  bgeu t0, t1, 2f
    sd   zero, 0(t0)
    addi t0, t0, 8
    j    1b
2:

    /* vetor de traps em modo-M */
    la   t0, trap_entry
    csrw mtvec, t0
    /* mscratch = topo da pilha de trap do hart 0 (trap.S troca sp) */
    la   t0, _pilha_trap0
    li   t1, 16384
    add  t0, t0, t1
    csrw mscratch, t0
    call kmain
1:  wfi
    j 1b

hart_secundario:
    la   t0, liberar_harts
1:  lw   t1, 0(t0)
    beqz t1, 1b
    /* UART 0x10000000: imprime HART1_OK caractere a caractere */
    la   a0, msg_hart1
2:  lbu  t1, 0(a0)
    beqz t1, 3f
    li   t2, 0x10000000
    sb   t1, 0(t2)
    addi a0, a0, 1
    j    2b
3:  la   t0, hart1_ok
    li   t1, 1
    sw   t1, 0(t0)
4:  wfi
    j    4b

    .section .rodata
msg_hart1:
    .asciz "HART1_OK\n"
