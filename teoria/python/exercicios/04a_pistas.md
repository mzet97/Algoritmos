---
layout: default
---

# Pistas da aula 04a

1. Para a prova, escreva a fatia `valores[ini:fim]` antes da primeira iteração. Depois substitua `(ini, fim)` pelos novos limites em cada ramo e use a ordenação para justificar o descarte. No fim, descubra o tamanho da fatia.
2. No grafo com ciclo, marque o vértice como visitado **antes** de colocá-lo na fila, não depois de retirá-lo. Conte quantas vezes cada vértice entra na fila. Se o destino não existe, qual condição esvazia a fila?
3. `alvo in valores` também é busca linear, mas escrita em C dentro do interpretador. Compare o custo por tentativa das duas referências antes de concluir qual teste é mais barato.
4. A razão entre os tempos cresce com o tamanho. Fixe a razão 100x primeiro e ajuste o tamanho até alcançá-la; registre CPU, Python e número de repetições junto com os tempos.
