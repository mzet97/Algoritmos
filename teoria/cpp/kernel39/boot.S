/* Boot RISC-V (QEMU virt, -bios none) para a aula 39: como o boot.S das
   aulas 33-38, mas o hart secundario executa C (hart1_conta) em vez de
   so anunciar e estacionar. Hart 0 segue para kmain; o secundario espera
   o sinal, conta sob o spinlock e anuncia HART1_OK. */
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
    call hart1_conta
2:  wfi
    j    2b
