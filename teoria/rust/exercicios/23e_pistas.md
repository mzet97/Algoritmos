---
layout: default
---

# Aula 23e: pistas graduais

Volte aos [exercícios](../23e_afinidade_e_numa.md#6-exercícios-obrigatórios) antes de abrir as soluções.

1. Preso a 1 núcleo, o toque vai a ~6 GB/s e a razão a ~0,29× (verificado). Oito threads disputando um núcleo: troca de contexto em vez de paralelismo. A linha `Cpus_allowed_list: 0` é o programa lendo a jaula onde o SO o pôs — afinidade por fora (`taskset`), observada por dentro (`/proc`). Programa não fixa onde roda; declara onde foi posto.
2. Sem o `assert`, modos divergentes relatariam razão normalmente — número sobre areia. O checksum igual é o que autoriza comparar tempos: prova que os dois modos calcularam a mesma coisa. Medir antes de provar igualdade é cronometrar dois programas diferentes e chamar de comparação.
3. Toque com 1 thread: ~6 GB/s; com 8: ~21 GB/s — speedup ~3,5× (verificado), não 8×. Triade é memory-bound: o teto é a banda, e thread além da banda é fila no controlador. Speedup sublinear em kernel de memória é saúde, não defeito — linearidade aqui seria suspeita (cache, não RAM).
4. Ordem invertida: razão ~1,0× em vez de 0,79× (verificado). Quem roda primeiro paga as faltas de página do vetor `a` (fresco no primeiro modo, morno no segundo): ~2 ms para 16k páginas, na mesma ordem da "vantagem" medida. Viés de ordem + ruído explicam tudo — e "explicam tudo" é o veredito honesto com 1 nó: sem o segundo nó, o laboratório mede a si mesmo.
