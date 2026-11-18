---
layout: default
---

# Aula 23e: soluções comentadas

Compare após tentar os [exercícios](../23e_afinidade_e_numa.md#6-exercícios-obrigatórios). Experimentos sobre `teoria/rust/exemplos/23e_afinidade.rs` com `rustc --edition 2024 -O -D warnings`.

1. Previsão: preso a 1 núcleo (`Cpus_allowed_list: 0`), o toque cai para **~6,5 GB/s** e a razão vai a **~0,29×** (verificado). Oito threads num núcleo só: troca de contexto em vez de paralelismo. A linha de CPUs é o programa lendo a jaula onde o SO o pôs — afinidade aqui entra por fora (`taskset`) e é observada por dentro (`/proc`); o programa não fixa onde roda, declara onde foi posto. Sem essa linha, o número seria órfão de contexto.

2. Diagnóstico: com matemática diferente por modo (`S_BUG` no tardio) e sem o `assert`, o relatório sai normalmente — razão 0,85× e até o "23e ok" (verificado). Número sobre areia: cronometrou dois programas diferentes e chamou de comparação. O `assert_eq!` dos checksums é o que autoriza comparar tempos — prova que os dois modos calcularam a mesma coisa. Medir antes de provar igualdade é o erro metodológico que o `assert` existe para impedir; sem ele, o relatório mente com a mesma cara de sempre.

3. Projeto: toque com 1 thread dá **~6 GB/s**; com 8, **~21 GB/s** — speedup ~3,5×, não 8× (verificado). Triade é memory-bound: o teto é a banda do controlador, e thread além da banda é fila. Speedup sublinear em kernel de memória é saúde — linearidade aqui seria suspeita (mediria cache, não RAM). A conclusão vale para qualquer máquina: paralelize o cálculo, mas o limite quem dá é o fio.

4. Previsão: ordem invertida (tardio primeiro) dá razão **~1,0×** em vez de 0,79× (verificado). Quem roda primeiro paga as faltas de página do vetor `a` (fresco no primeiro modo, morno no segundo): ~2 ms para 16k páginas, na mesma ordem de grandeza da "vantagem" medida. Viés de ordem + ruído explicam toda a diferença — e "explicam tudo" é o veredito honesto com 1 nó: sem o segundo nó, o laboratório mede a si mesmo (páginas, ordem, ruído), e dizê-lo é o resultado da aula, não a falta dele.
