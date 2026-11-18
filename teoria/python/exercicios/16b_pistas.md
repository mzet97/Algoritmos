---
layout: default
---

# Pistas da aula 16b

1. Tres locks em ciclo A->B->C->A: a ordem global e qualquer ordem total fixa dos tres.
2. acquire(timeout) com repeticao: conte os timeouts; o programa 'funciona' pagando espera para sempre. Mascara = o sintoma some e a causa fica.
3. Fila ilimitada com produtor rapido: RSS sobe linearmente com o atraso do consumidor; a limitada troca isso por espera do produtor.
4. Com multiprocessing, um worker travado nao trava o pai: o isolamento compra supervisao (matar e reiniciar o processo), ao preco de serializar tudo que cruza.
