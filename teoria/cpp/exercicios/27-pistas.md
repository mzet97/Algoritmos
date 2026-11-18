---
layout: default
---

# Aula 27 (C++): pistas graduais

Volte aos [exercicios](../27.md#21-exercicios-obrigatorios) antes de abrir as solucoes.

1. `request_id` entra no contexto da requisicao e sai em **todas** as linhas de log dela, nao so na primeira.
2. Histograma de latencia: faixas fixas (buckets) e um contador por faixa; percentil se estima pela faixa acumulada.
3. `ScopedSpan` abre na construcao e fecha no destrutor — inclusive em retorno antecipado e excecao.
4. Metricas basicas de servidor TCP: conexoes ativas, total aceitas, erros por classe, latencia e bytes.
5. Logs contam a historia de **um** caso; quando a pergunta e "quantos, quao rapido, piorando?", texto nao agrega.
