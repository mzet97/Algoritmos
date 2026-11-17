# Pistas da aula 30a

1. Plote razao (eixo y) contra threads (eixo x) para n = 1200. O achatamento aparece quando somar mais uma thread nao adiciona banda: a matriz inteira atravessa a memoria a cada chamada, e banda e compartilhada.
2. Ao repartir a linha, cada thread produz uma soma parcial: o resultado final soma parciais numa ordem que nao e a sequencial. A igualdade bit a bit morre ai; a pos-condicao vira tolerancia relativa contra uma referencia em double.
3. No pool, as threads nascem uma vez e esperam trabalho numa fila. O que some em n = 40 e o custo de criacao+juncao por chamada; o que fica e o custo de acordar e sincronizar, bem menor.
4. float corta os bytes por elemento pela metade (8 para 4). Se o gargalo e banda, os dois lados aceleram parecido; a razao seq/par muda pouco, mas o tempo absoluto cai — banda manda nos dois.
