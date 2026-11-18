---
layout: default
---

# Aula 27 (Go): pistas graduais

Volte aos [exercicios](../27.md#25-exercicios-obrigatorios) antes de abrir as solucoes.

1. `GODEBUG=gctrace=1` imprime uma linha por coleta: compare com e sem carga.
2. Payload temporario maior significa mais bytes vivos por unidade de tempo: `HeapAlloc` e `NumGC` contam.
3. Um worker serializa o trabalho: throughput cai, latencia de fila sobe — meça os dois.
4. Fila menor enche antes: backlog vira descarte mais cedo, nao mais tarde.
5. Escape se procura no `-gcflags="-m"` como nas aulas 09 e 17: nomeie a causa.
6. Goroutine e unidade de espera, nao de CPU: paralelismo real mora nos nucleos.
