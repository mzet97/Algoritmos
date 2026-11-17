# Pistas da aula 23c

1. Kahan carrega a compensacao do erro de cada soma; compare erro contra f64 e o tempo contra os 4 acumuladores (a cadeia de Kahan e serial).
2. Varra 2, 4, 8 acumuladores no mesmo vetor: a razao satura quando banda ou largura do pipeline manda; anote onde.
3. First-touch desenhado: alocar e tocar as paginas com a thread fixada no no 0, medir o somatorio com a thread no no 1. Nesta maquina (1 no) fica no papel, rotulado.
4. rustc --emit asm e procure instrucoes packed (addps/mulps ou variantes AVX) no laco dos acumuladores; ausencia com ganho presente indica ILP escalar, nao vetorizacao.
