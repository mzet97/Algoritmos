# Pistas da aula 29c

1. Cronometre so o pickle dos blocos (dumps/loads) sem computar: esse e o termo de viagem dos processos.
2. Threads na mesma carga CPU-bound em Python puro: o GIL serializa; o speedup some. Com NumPy liberando o GIL em operacoes vetorizadas, a historia muda — carga decide.
3. Varra o tamanho ate o pool empatar com o sequencial: abaixo dele, processo e imposto.
4. Amdahl com termo fixo: 1/((1-p) + p/s + custo_fixo/trabalho). O micro caso e esse termo mandando.
