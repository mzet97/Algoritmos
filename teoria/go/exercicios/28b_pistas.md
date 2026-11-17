# Pistas da aula 28b

1. Kahan compensa o erro por soma; os 4 acumuladores quebram a cadeia. Erro e tempo contam historias opostas.
2. Sob -race cada acesso ganha instrumentacao: os dois lados pagam, mas nao na mesma proporcao. Meça e reporte separado.
3. First-touch desenhado: tocar as paginas com a thread fixada no no onde o calculo vai rodar; aqui, 1 no, fica no papel rotulado.
4. Laço por indices remove o iterador: veja se o compilador ainda reconhece o padrao de reducao (bounds checks eliminados ou nao).
