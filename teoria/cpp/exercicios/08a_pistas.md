# Pistas da aula 08a

1. O perigo do `size_t`: `fim - ini` nunca é negativo enquanto `ini <= fim` — a prova precisa mostrar que os dois ramos mantêm `ini <= fim`. Se um ramo fizesse `fim = meio - 1` com `meio == 0`, o sem-sinal transformaria `-1` no maior `size_t` do mundo.
2. Ordene por `chave` com comparador que ignora `seq`; depois verifique, para cada chave repetida, se `seq` saiu crescente. A implementação que falhar é a não estável — confira contra a documentação qual das três promete estabilidade.
3. Em `n` pequeno, o custo fixo do mergesort (recursão, buffer) domina e a inserção, que é um laço simples e cache-amigável, vence. Plote os três tamanhos pequenos ao lado dos grandes: o cruzamento é a resposta, não um número decorado.
4. Monte o contraexemplo mínimo: vetor `{2, 1}` sem ordenar, alvo `2`. A busca compara com o meio e descarta a metade onde o 2 está. Quem verifica: se a função não pode ordenar (custo) nem conferir (custo), o contrato documenta a pré-condição e o teste de quem chama a cumpre.
