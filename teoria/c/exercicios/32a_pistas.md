# Pistas da aula 32a

1. A syscall nova mora no mesmo `case 9`: leia `a0` do frame (`f[F_A0]`), imprima pelo `kputc` do modo-M e avance `mepc`. Depois apague o acesso direto das tarefas à UART — elas passam a depender do kernel para falar, que é exatamente a fronteira que um SO existe para criar.
2. Quantum é o `INTERVALO_TICKS` e a regra de troca (hoje, toda interrupção). Com quantum curto, conte quantas trocas cabem no mesmo período de impressão; o custo aparece como menos caracteres por segundo com o mesmo número de ticks.
3. O endereço novo cai no `default` das faltas: confira se `tval` bate com o novo endereço antes de culpar o mapa. Desligar é certo porque mapear endereço que nenhuma etapa do contrato previu transformaria qualquer ponteiro selvagem em página válida.
4. Para 4 KiB, a entrada da raiz vira ponteiro para uma tabela L1, que aponta para uma L0. Conte: 1 (raiz) + entradas L1/L0 só do intervalo da pilha. O que se compra é granularidade de proteção; o que se paga é memória de tabelas e passos de tradução (TLB à parte).
