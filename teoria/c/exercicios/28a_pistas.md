---
layout: default
---

# Pistas da aula 28a

1. Hipótese honesta para soma em streaming: o barramento entrega X GB/s e cada soma lê 8 bytes; o teto da soma paralela é banda dividida, não núcleos multiplicados. Calcule o teto antes de medir.
2. Rode com 1, 2, 4 e 8 threads e plote speedup contra núcleos. Se a curva achata cedo, o gargalo é compartilhado (banda); se escala e depois cai, procure contenção ou turbo.
3. `alignas(64)` por contador: cada thread escreve só na sua linha. Compare o tráfego de invalidação nos dois casos com um contador de performance, se disponível (`perf stat -e cache-misses`).
4. Some `t_ida + t_kernel + t_volta` com a banda PCIe medida (não a do folheto) e o tempo de kernel estimado. A conta diz o speedup mínimo que justifica a viagem; abaixo dele, nem aceleração infinita compensa.
