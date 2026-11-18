---
layout: default
---

# Soluções comentadas da aula 30b

## 1. Kahan contra quatro acumuladores

A soma compensada de Kahan reduz o erro para perto do da referência em `double` (tipicamente abaixo de 1e-6 relativo neste experimento, melhor que os 1.00e-03 dos quatro acumuladores), porque ela recupera explicitamente os bits perdidos a cada adição. O custo: a compensação cria uma cadeia de dependência serial — cada iteração espera a anterior — e o tempo volta para perto do escalar. Conclusão que a aula quer: precisão e velocidade trocam entre si, e a escolha (Kahan, 4 acumuladores, `double`, AVX) se faz declarando o modelo de erro aceitável, não por gosto.

## 2. Afinidade e dispersão

Com a thread livre, repetições ocasionais caem em outra CPU com cache frio: a média se move pouco, mas o máximo dispara. Fixada, a razão máx/mín encolhe visivelmente. O número que a afinidade compra é **previsibilidade** (cauda), não necessariamente média — é por isso que sistemas de tempo real fixam threads mesmo sem ganho médio.

## 3. `-ffast-math`

O flag autoriza: reordenação associativa das somas (o compilador pode criar os quatro acumuladores por você — e mudar seu resultado), contração multiplicação+adição em FMA, e a presunção de que NaN/Inf não existem. Se os erros contra o oráculo mudam, a semântica numérica do programa mudou sem uma linha sua mudar. Frase de README honesta: "este projeto compila com `-ffast-math`; resultados em ponto flutuante podem diferir entre compiladores e não tratam NaN/Inf".

## 4. First-touch desenhado, não executado

No Linux, página física nasce no nó da CPU que a toca primeiro (first-touch). Experimento: thread fixada no nó 0 inicializa os vetores; medir o produto interno com a thread no nó 0 (local) e depois no nó 1 (remoto) mostraria a penalidade de banda/latência entre nós. Nesta máquina há 1 nó (`nos_numa=1` no exemplo), logo o experimento está apenas desenhado — declarar isso vale mais que simular com processos e chamar de NUMA.
