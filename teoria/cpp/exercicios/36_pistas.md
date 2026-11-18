---
layout: default
---

# Aula 36: pistas graduais

Volte aos [exercícios](../36.md#7-exercícios-obrigatórios) antes de abrir as soluções.

1. A 2ª cai em `SYSCALL_DESCONHECIDA a7=2` + `0x3333`. A guarda `!echo_ok` fez da syscall testemunha única: o primeiro uso consome a permissão. Programa real chamaria eco N vezes — syscall didática com guarda é experimento, não API. Idempotência de verdade estaria no handler (atender sempre igual), não na trava de demonstração.
2. A sonda continua causa 2 (U tampouco lê `mstatus`), mas o `ecall` vira **causa 8** — e o handler só trata 9: `TRAP_INESPERADA causa=8`. Mesmo `ecall`, causas distintas por privilégio de origem. O número da causa diz *de onde* veio a chamada; tratar 8 como 9 seria atender usuário com protocolo de supervisor.
3. `f[9]` é `a0` (x10), `f[10]` é `a1` (x11) — o frame espelha `x1..x31` em `f[0..30]`. Somar `f[9] + f[10]` e devolver em `f[9]` é a convenção inteira: entrada e saída viajam nos mesmos slots. Índice errado lê registrador errado sem aviso — e o teste (7 + 35 = 42) é o que prende o mapa.
4. **0**, sempre (o handler escreve `tf->f[F_A0] = 0`). Anunciar ignora a entrada por contrato: cada número de syscall define o que entra e o que sai. "Retorna o que entrou" só vale para o eco — presumir para as demais é inventar ABI.
