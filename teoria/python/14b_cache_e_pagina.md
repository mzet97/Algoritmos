---
layout: default
---

# Aula 14b — Cache e página vistos de Python: duas representações, duas físicas

**Competências SDD:** ARQ, MEM, OS. **Pré-requisitos:** [14a](14a_do_interpretador_ao_kernel.md) (bytecode, RSS contra `tracemalloc`, syscalls) e 13/17. Esta ponte completa o PY4: hierarquia de cache e página medidas de dentro de um programa Python.

**Objetivo observável:** medir a mesma soma por linhas e por colunas em duas representações — lista de listas e `array('l')` plano —, ler o tamanho de página do SO (`SC_PAGE_SIZE`) e o pico de RSS (`ru_maxrss`), e explicar por que as razões diferem entre as representações.

**Contrato e exemplo:** somas idênticas conferidas por asserção antes de qualquer cronômetro. O [arquivo-fonte](exemplos/14b_cache.py) é o programa completo:

```bash
python3 teoria/python/exemplos/14b_cache.py
# lista de listas: linhas=0.0025s colunas=0.0262s razao=10.38x
# array plano: sequencial=0.0396s stride=0.0472s razao=1.19x
# pagina=4096 bytes rss_max=... KiB
# 14b ok: ...
```

**Leitura:** na lista de listas, cada linha é um objeto separado e cada elemento é um objeto `int`: o percurso por colunas paga busca de objetos espalhados (10×), mas o custo dominante é o interpretador, não a linha de cache — razão menor e mais suja que a de C pelo mesmo fenômeno. No `array` plano e contíguo, o stride de 900 elementos ainda cabe em poucas páginas e o efeito cai para 1,19×: a física do cache existe, mas em Python ela aparece **atenuada pelo custo por operação do interpretador**, que domina a conta. Quem medir só em Python conclui que localidade "importa pouco"; quem mediu em C/Rust sabe que importa — a linguagem é que esconde. `ru_maxrss` é **pico** de RSS, não residência atual, e a aula nomeia isso para o relatório não mentir com um número certo da coisa errada.

**Erros comuns:** chamar lista de listas de "matriz contígua"; atribuir a razão ao cache sem separar o custo do interpretador; reportar `ru_maxrss` como uso atual; medir uma vez sem conferir a igualdade das somas.

**Exercícios:** [pistas](exercicios/14b_pistas.md) e [soluções](exercicios/14b_solucoes.md): (1) repetir com NumPy (se disponível) e mostrar a razão "de hardware" reaparecendo; (2) varrer tamanhos e achar onde a razão da lista de listas nasce; (3) medir RSS atual por `/proc/self/status` (VmRSS) contra o pico do `ru_maxrss` e explicar a diferença; (4) desenhar o caminho Python → objeto → memória para um único `m[i][j]`.

**Checklist:** igualdade conferida antes de medir; as duas representações interpretadas separadamente; pico ≠ atual sem hesitar; página lida do SO, não decorada.
