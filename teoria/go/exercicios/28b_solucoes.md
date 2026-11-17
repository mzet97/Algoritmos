# Solucoes comentadas da aula 28b

Kahan: erro perto do float64, tempo de soma serial. 4 acumuladores: erro ~10x menor que o sequencial aqui e tempo igual porque o compilador ja fazia o trabalho — o empate e o resultado. Sob -race, instrumentacao domina e a razao muda: nunca misture as tabelas. First-touch sem segundo no e desenho, nao medicao. Indice manual pode reintroduzir bounds check por iteracao se o compilador nao provar os limites; range existe tambem como informacao para o otimizador.
