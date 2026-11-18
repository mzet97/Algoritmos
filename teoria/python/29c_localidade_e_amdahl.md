---
layout: default
---

# Aula 29c — Localidade, processos e Amdahl: o teto antes do entusiasmo

**Competências SDD:** HPC, GPU, ARQ, MEM. **Pré-requisitos:** [29a](29a_hpc_e_modelo_gpu.md) (baseline escalar, NumPy, threads/processos e conta da viagem) e [14b](14b_cache_e_pagina.md). Esta ponte completa o PY8 sobre a base da 29a.

**Objetivo observável:** conferir a referência sequencial antes de paralelizar, medir o speedup real com `multiprocessing.Pool` (limitado a 8 processos), calcular os tetos de Amdahl para três frações paralelizáveis e demonstrar o caso em que o pool perde por três ordens de grandeza (trabalho microscópico).

**Contrato e exemplo:** corretude por soma conferida contra a referência; speedup só com os dois tempos e o número de processos declarados. O [arquivo-fonte](exemplos/29c_localidade.py):

```bash
python3 teoria/python/exemplos/29c_localidade.py
# seq=0.221s processos(8)=0.205s speedup=1.08x
# p=0.9: teto com 8 nucleos = 4.71x (infinito nucleos = 10.0x)
# micro: seq=0.02ms pool=24.8ms (paralelo perde por criacao de processos)
# 29c ok: ...
```

**Leitura:** o speedup medido é 1,08×, não 8× — e a conta explica: os 3 milhões de floats viajam serializados (pickle) para os processos e os resultados voltam; a "viagem" da aula de GPU existe aqui dentro da mesma máquina, com outro nome. Amdahl formaliza o resto: com 90% paralelizável, nem infinitos núcleos passam de 10×, e com 8 o teto é 4,71× — antes de contar serialização. O caso micro (0,02 ms de trabalho contra 24,8 ms de pool) é o mesmo teorema da 30a de C++ com threads, agora com processos: coordenação tem preço fixo, e abaixo dele paralelizar é um imposto. Localidade entra como a pergunta que decide **antes** do pool: os blocos são fatias contíguas dos dados (não amostras com stride), porque processo nenhum conserta um padrão de acesso ruim — só o replica em 8 lugares.

### A conta completa do 1,08× (Amdahl + serialização + réplica)

Três termos explicam o número, e a aula os separa porque cada um manda em um regime: (a) Amdahl — com `p=0,9` e 8 núcleos, o teto é 4,71× mesmo com transporte grátis; é o limite da fração serial, e nenhuma engenharia o move sem paralelizar o serial; (b) serialização — os blocos viajam por pickle na ida e na volta, e esse custo entra inteiro no tempo paralelo sem existir no serial: é o termo que derruba 4,71× para 1,08×, e o exercício 1 manda medi-lo isolado para provar a atribuição; (c) réplica do padrão de acesso — 8 processos com stride ruim são 8× o miss, não a correção dele; fatia contígua por bloco é pré-requisito, e a aula o cobra antes do pool (localidade decide antes, pool executa depois). O caso micro zera (a) e (b) sobrando só (c fixo): 0,02 ms de trabalho contra 24,8 ms de criação — e 24,8 ms é o preço de nascer processo, não de computar. Quem soma os três antes de medir prevê 1,08× sem surpresa; quem mede sem somar chama de "overhead misterioso".

### Leitura guiada: referência, teto, imposto

`seq=0.221s processos(8)=0.205s speedup=1.08x` é a referência conferida antes (contrato: soma igual, depois relógio) e o speedup com ambiente declarado (8 processos, sem `-race` escondido). A linha de Amdahl (`p=0.9: teto 4.71x, infinito 10.0x`) é a conta antes do entusiasmo: publicada junto da medida para que ninguém compare 1,08× com 8× — a comparação honesta é 1,08× contra 4,71× (teto), e a distância entre eles é a serialização nomeada. `micro: seq=0.02ms pool=24.8ms` é o imposto: três ordens de grandeza de coordenação sobre trabalho que já tinha acabado — o mesmo teorema da 30a de C++, com `fork` no lugar de `thread`. Três linhas, três perguntas respondidas: quanto deu, quanto poderia dar, quando perde.

**Erros comuns:** prometer `cpu_count()`× de speedup; medir sem conferir a soma; ignorar o custo de serializar a entrada (o pickle é a PCIe dos processos); pool com processos demais para o tamanho do trabalho; chamar o teto de Amdahl de pessimismo em vez de conta.

**Exercícios:** [pistas](exercicios/29c_pistas.md) e [soluções](exercicios/29c_solucoes.md): (1) medir o tempo só de serialização dos blocos e subtrair do "tempo paralelo"; (2) trocar processos por threads na mesma carga e explicar o resultado pelo GIL e pela natureza da carga (CPU em Python puro); (3) achar o tamanho mínimo em que o pool empata; (4) recalcular Amdahl incluindo um termo fixo de coordenação medido.

**Checklist:** referência conferida antes; speedup com ambiente e processos declarados; teto de Amdahl calculado, não citado; o caso que perde medido; serialização contabilizada.
