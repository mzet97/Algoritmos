# Soluções comentadas da aula 29a

## 1. Razão em função de `n`

Típico: em `n` pequeno, processos perdem feio (spawn + IPC dominam); crescendo `n`, a razão melhora até um platô ditado por banda de memória. Se a curva não monotonicamente melhora, suspeite de turbo, troca de contexto ou máquina compartilhada — anote a carga e repita.

## 2. `np.sqrt` contra laço com `math.sqrt`

O laço paga bytecode + GIL por elemento; `np.sqrt` paga um kernel C vetorizado com GIL liberado. A razão costuma ser maior que na soma de quadrados, porque há mais trabalho por elemento para vetorizar (divisão/raiz têm latência alta em escalar). Mesma conclusão, mecanismo mais visível.

## 3. Ponto de equilíbrio

Procedimento: dobre `n` a partir de 100 mil; para cada, meça NumPy puro contra processos (4 workers) com igualdade verificada. O empate tipicamente cai entre 1 e 10 milhões de elementos nesta classe de máquina — publique CPU, RAM, NumPy e SO, pois o número não viaja entre máquinas.

## 4. Conta que vira o veredito

`2 * bytes / banda + t_kernel < t_cpu`. Com 8 MB: a 16 GiB/s, a viagem custa ~1 ms — o kernel precisa ser ordens de magnitude mais rápido que os ~1 ms do NumPy para compensar. A 100 GiB/s e kernel de 10 ms, a viagem cai para ~0,16 ms e a conta vira: 10,16 ms contra ~1 ms... continua perdendo, porque o denominador (trabalho) é pequeno demais. Moral: GPU compensa trabalho grande e aritmeticamente denso, não micro-operação.
