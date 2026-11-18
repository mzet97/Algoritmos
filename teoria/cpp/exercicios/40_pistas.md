---
layout: default
---

# Pistas da aula 40

1. A entrada 0x80 segue o molde da entrada 3 do componente: mesmo seletor, tipo com DPL compativel com int por software, handler lendo RAX como numero. O byte a imprimir pode viajar em RDI, como o argumento da syscall da 33 viajava em registrador.
2. A linha mais fragil costuma ser a do timer ou a da pilha de trap: confira no comportamento — quem salva o que, e quando — antes de defender a correspondencia.
3. INIT/SIPI: o nucleo principal acorda o secundario com inter-processor interrupts via APIC, apontando uma pagina de trampolim abaixo de 1 MiB em modo real. O hart RISC-V acorda porque o reset larga todos os harts no mesmo _start; x86 larga os outros dormindo por desenho.
4. Do trap.S, o equivalente conceitual no int3 e salvar/restaurar registradores em volta do tratador. O que a CPU x86 ja empilha (RIP/CS/RFLAGS, e codigo de erro em algumas excecoes) e trabalho que no RISC-V e manual via mepc/mcause — flexibilidade de um lado, cerimonia do outro.
