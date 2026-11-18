---
layout: default
---

# Soluções comentadas da aula 30a

## 1. Linhas de cache por iteração

O ingênuo, ao computar `C[i][j]`, percorre a coluna `j` de `B`: cada `B[k][j]` está em linha diferente (passo de `n * 8` bytes), então cada iteração do laço interno paga quase uma linha inteira para usar 1 double. O tiled carrega o bloco `BxB` uma vez e o reusa `B` vezes: o custo de memória por operação cai pelo fator de reuso. É exatamente o papel da memória compartilhada no modelo GPU: reuso explícito, não mágica.

## 2. Tamanho de bloco

Típico em desktop: 8 e 16 sofrem com overhead de laço e pouco reuso; 32 costuma ser o ponto doce para L1 de 32–48 KiB; 64 pode regredir se dois blocos deixarem de caber juntos. O número exato varia com a cache real — publique L1 da sua máquina junto (ex.: `/sys/devices/system/cpu/cpu0/cache/`).

## 3. Conta da viagem

```text
t_total = 2 * bytes / banda + t_kernel
```

Com 800 MiB e 16 GiB/s: `t_total = 0,093 + t_kernel`. Para bater CPU de 0,2 s, precisa `t_kernel < 0,107 s` — speedup de quase 2x *só para empatar*, antes de contar lançamento e sincronização. Abaixo disso, "não compensa mover" é a resposta certa, e ela também é engenharia.

## 4. Condição dependente dos dados

Com `mexido[i] % 3 == 0`, ordenado e embaralhado voltam a divergir — mas a direção pode surpreender: o custo está na *previsibilidade do padrão*, não na "complexidade" da condição. É por isso que o SIMT sofre com divergência de dados, não com `if`s em si.
