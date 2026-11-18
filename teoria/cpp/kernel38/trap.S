/* Entrada de trap em modo-M: troca sp com mscratch (pilha de trap),
   salva x1..x31 e mepc num TrapFrame (ver kernel.c), chama o tratador em
   C e restaura. O tratador pode devolver OUTRO frame (troca de tarefa):
   quem decide a proxima tarefa e o C; aqui so restauramos o que vier. */
    .section .text
    .globl trap_entry
    .align 2
trap_entry:
    csrrw sp, mscratch, sp
    addi sp, sp, -264
    sd x1, 0(sp)
    sd x3, 16(sp)
    sd x4, 24(sp)
    sd x5, 32(sp)
    sd x6, 40(sp)
    sd x7, 48(sp)
    sd x8, 56(sp)
    sd x9, 64(sp)
    sd x10, 72(sp)
    sd x11, 80(sp)
    sd x12, 88(sp)
    sd x13, 96(sp)
    sd x14, 104(sp)
    sd x15, 112(sp)
    sd x16, 120(sp)
    sd x17, 128(sp)
    sd x18, 136(sp)
    sd x19, 144(sp)
    sd x20, 152(sp)
    sd x21, 160(sp)
    sd x22, 168(sp)
    sd x23, 176(sp)
    sd x24, 184(sp)
    sd x25, 192(sp)
    sd x26, 200(sp)
    sd x27, 208(sp)
    sd x28, 216(sp)
    sd x29, 224(sp)
    sd x30, 232(sp)
    sd x31, 240(sp)
    csrr t0, mepc
    sd t0, 248(sp)
    /* sp original (da tarefa) estava em mscratch apos a troca */
    csrr t0, mscratch
    sd t0, 8(sp)

    mv a0, sp
    call trap_handler
    /* a0 = frame a restaurar (pode ser de outra tarefa) */
    mv sp, a0
    ld t0, 248(sp)
    csrw mepc, t0
    ld t0, 8(sp)
    /* mscratch volta ao topo fixo da pilha de trap (nunca ao frame) */
    la t1, _pilha_trap0
    li t2, 16384
    add t1, t1, t2
    csrw mscratch, t1
    ld x1, 0(sp)
    ld x3, 16(sp)
    ld x4, 24(sp)
    ld x5, 32(sp)
    ld x6, 40(sp)
    ld x7, 48(sp)
    ld x8, 56(sp)
    ld x9, 64(sp)
    ld x10, 72(sp)
    ld x11, 80(sp)
    ld x12, 88(sp)
    ld x13, 96(sp)
    ld x14, 104(sp)
    ld x15, 112(sp)
    ld x16, 120(sp)
    ld x17, 128(sp)
    ld x18, 136(sp)
    ld x19, 144(sp)
    ld x20, 152(sp)
    ld x21, 160(sp)
    ld x22, 168(sp)
    ld x23, 176(sp)
    ld x24, 184(sp)
    ld x25, 192(sp)
    ld x26, 200(sp)
    ld x27, 208(sp)
    ld x28, 216(sp)
    ld x29, 224(sp)
    ld x30, 232(sp)
    ld x31, 240(sp)
    ld x2, 8(sp)
    mret

/* Entrada na primeira tarefa: restaura o frame inicial e da mret. */
    .globl lancar_tarefa
lancar_tarefa:
    mv sp, a0
    ld t0, 248(sp)
    csrw mepc, t0
    addi t1, sp, 264
    la   t2, _pilha_trap0
    li   t3, 16384
    add  t2, t2, t3
    csrw mscratch, t2
    ld x1, 0(sp)
    ld x3, 16(sp)
    ld x4, 24(sp)
    ld x5, 32(sp)
    ld x6, 40(sp)
    ld x7, 48(sp)
    ld x8, 56(sp)
    ld x9, 64(sp)
    ld x10, 72(sp)
    ld x11, 80(sp)
    ld x12, 88(sp)
    ld x13, 96(sp)
    ld x14, 104(sp)
    ld x15, 112(sp)
    ld x16, 120(sp)
    ld x17, 128(sp)
    ld x18, 136(sp)
    ld x19, 144(sp)
    ld x20, 152(sp)
    ld x21, 160(sp)
    ld x22, 168(sp)
    ld x23, 176(sp)
    ld x24, 184(sp)
    ld x25, 192(sp)
    ld x26, 200(sp)
    ld x27, 208(sp)
    ld x28, 216(sp)
    ld x29, 224(sp)
    ld x30, 232(sp)
    ld x31, 240(sp)
    ld x2, 8(sp)
    mret
