# Soluções comentadas da aula 28a

## 1. Teto de banda da soma

800 MB por passada (100M × 8 bytes). Com banda efetiva de B GB/s por núcleo-socket, o piso do tempo sequencial é `0.8/B` segundos; 4 threads dividem o mesmo barramento e o piso cai para `0.8/min(B_total, ...)`. Na prática desta máquina, o speedup medido fica bem abaixo de 4 — e é a banda, não o código, que manda. Some `STREAM`-like: sem medir banda, qualquer alegação de "escala mal" é chute.

## 2. Curva 1-2-4-8 e o joelho

Esperado típico em desktop: ~1x, ~1.8x, ~2.5-3x, ~3x (achatando). O achatamento marca onde a banda saturou; queda em 8 pode ser turbo reduzido, afinidade ou carga alheia. Registre `nproc`, governador e carga junto — sem isso a curva não é evidência.

## 3. `perf stat` nos dois regimes

```bash
perf stat -e cache-misses,cache-references ./bench28a_vizinhos
perf stat -e cache-misses,cache-references ./bench28a_isolados
```

Os vizinhos mostram taxa de miss por acesso ordens acima: cada escrita invalida a linha para os demais. Isolados mostram miss só no aquecimento. Sem `perf` (contêiner/VM sem contadores), a razão de tempos já conta a história — mas declare a ausência do contador.

## 4. Conta da viagem para o acelerador

```text
t_total = bytes / banda_pcie + t_kernel + bytes_resultado / banda_pcie
```

Com 800 MB e 16 GB/s efetivos: só a viagem custa ~0,05 s por sentido. Se o kernel na CPU leva 0,2 s, o acelerador precisa entregar o mesmo trabalho em bem menos de 0,1 s para compensar — speedup real exigido acima de 2x *antes* de contar overhead de lançamento. Abaixo desse limiar, a resposta honesta é "não compensa mover".
