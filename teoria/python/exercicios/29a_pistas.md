# Pistas da aula 29a

1. Suba `n` e anote razão NumPy/processos em cada degrau. O ponto de virada é onde IPC deixa de dominar: marque-o com ambiente junto.
2. `np.sqrt` vetoriza; `math.sqrt` em laço não. Meça os dois e explique a razão com GIL + C, não com "NumPy é mágico".
3. Comece em `n` pequeno e dobre até processos empatarem. O número vale para a sua máquina, não como constante universal — publique os dois.
4. Troque banda e kernel na fórmula e ache o par (banda, kernel) em que o veredito vira. Discuta se esse par existe no mundo real.
