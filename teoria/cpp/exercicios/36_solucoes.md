---
layout: default
---

# Aula 36: soluções comentadas

Compare após tentar os [exercícios](../36.md#7-exercícios-obrigatórios). Variantes sobre `teoria/cpp/kernel36/`, compiladas com o `Makefile` do estágio e rodadas no QEMU.

1. Previsão: a 2ª **não** imprime `ECHO_OK` — cai em `SYSCALL_DESCONHECIDA a7=2` + `0x3333` (verificado). A guarda `!echo_ok` consumiu a permissão no primeiro uso: syscall didática virou uso-único. Programa real precisaria ecoar N vezes — idempotência de verdade mora no handler (atender sempre igual), não na trava de demonstração. A trava prova o caminho uma vez; a API prova sempre.

2. Diagnóstico: a sonda continua **causa 2** (U também não lê `mstatus`), mas o `ecall` vira **causa 8** — e o handler só conhece 9: `TRAP_INESPERADA causa=0x8` (verificado). Mesmo `ecall`, causas distintas por privilégio de origem (U=8, S=9, M=11). O número da causa diz *de onde* veio a chamada; tratar 8 como 9 seria atender usuário com protocolo de supervisor — e é por isso que cada modo tem seu número: confundir os dois é defeito de protocolo, não de digitação.

3. Projeto: com `a7 = 3` somando `f[9] + f[10]` e a tarefa chamando com (7, 35): `SOMA_OK`, `DONE` (verificado). O mapa `f[]` ↔ registradores: `f[0]=x1, f[1]=sp(x2) ... f[9]=x10(a0), f[10]=x11(a1)` — o frame espelha `x1..x31` em ordem. Errar o índice lê o registrador errado em silêncio (sem trap, sem aviso — só número errado). O teste (7 + 35 = 42) é o que prende o mapa: aritmética conhecida detecta índice trocado.

4. Previsão: **0** — `ANUNCIA_IGNORA_ENTRADA` (verificado). O handler escreve `tf->f[F_A0] = 0` incondicionalmente: anunciar zera por contrato, independente da entrada. Cada número de syscall define seu contrato de retorno (anuncia→0, eco→a0+1); "retorna o que entrou" só vale onde o contrato diz. Presumir eco universal é inventar ABI — e ABI inventada quebra no primeiro programa real.
