# Pistas da aula 28a

1. Fração paralelizável perto de 1 com N workers: o teto é `1/((1-p)+p/N)` antes da contenção. Calcule para p=0,99 e N=2,4,8 e compare com o medido; a diferença é banda + overhead.
2. `byte` lê 8x menos bytes que `int64` para a mesma contagem: a banda rende mais, mas o trabalho por byte cai. Preveja antes de medir.
3. Some `len` por worker em vez de concatenar strings: alocação some, a invariante continua verificável. Meça os dois e explique a razão.
4. Troque banda e kernel na fórmula e ache o par que vira o veredito. Discuta se ele existe no mundo real para *esta* carga.
