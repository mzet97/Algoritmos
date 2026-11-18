---
layout: default
---

# Aula 14b — Cache e página vistos de Python: duas representações, duas físicas

## Objetivos verificáveis

Ao final, você deverá: distinguir duas representações e duas físicas (cache/página), prever efeito de layout no tempo, e medir sem alegar causa sem evidência.
Mediremos com níveis de proficiência:
- **Nível 1 — Recordar:** `definir linha de cache, página e layout`;
- **Nível 2 — Aplicar:** `prever padrão mais rápido e confirmar por medição`;
- **Nível 3 — Projetar:** `projetar layout justificando com números`.

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

### Duas representações, dois experimentos (por que as razões diferem)

A lista de listas mede localidade de **objetos**: por colunas, cada `m[i][j]` segue dois ponteiros (linha, depois elemento) para objetos espalhados pelo heap — 10,38× que mistura miss de cache com miss de localidade de alocação. O `array('l')` plano mede localidade de **bytes**: elementos contíguos de tipo fixo, e o stride de 900 só reordena o acesso dentro de páginas já residentes — 1,19×, quase só o cache falando. Comparar 10,38× com 1,19× como "duas medidas do mesmo fenômeno" erra a leitura: são dois fenômenos (dispersão de objetos vs. ordem de acesso) sob o mesmo nome. E o interpretador atenua os dois: cada operação carrega despacho, boxing e refcount — custo fixo por elemento que dilui qualquer razão de cache. A moral metodológica: em Python, razão pequena não significa efeito pequeno — significa efeito dividido por denominador grande. O exercício 1 (NumPy) remove o denominador e mostra a física sem atenuação.

### Leitura guiada: igualdade primeiro, números depois, pico nomeado

A igualdade das somas vem antes de qualquer cronômetro (contrato): medir representações que somam diferente seria comparar programas distintos. As duas razões vêm com a representação colada (`lista de listas` vs `array plano`) — número sem representação é o erro que a subseção acima desfaz. `pagina=4096` é lida do SO (`SC_PAGE_SIZE`), não decorada: página se consulta porque varia por plataforma, e decorar 4096 é apostar contra o futuro. `rss_max` carrega o aviso de pico: máximo atingido, não estado atual — relatório que cita pico como "uso" mente com número verdadeiro, e a aula prefere o número qualificado ao número liso.

**Erros comuns:** chamar lista de listas de "matriz contígua"; atribuir a razão ao cache sem separar o custo do interpretador; reportar `ru_maxrss` como uso atual; medir uma vez sem conferir a igualdade das somas; comparar as duas razões como mesmo fenômeno (objetos dispersos vs. bytes contíguos — nomes iguais, físicas distintas); decorar tamanho de página (varia por plataforma; `SC_PAGE_SIZE` existe para não apostar).

**Exercícios:** [pistas](exercicios/14b_pistas.md) e [soluções](exercicios/14b_solucoes.md): (1) repetir com NumPy (se disponível) e mostrar a razão "de hardware" reaparecendo; (2) varrer tamanhos e achar onde a razão da lista de listas nasce; (3) medir RSS atual por `/proc/self/status` (VmRSS) contra o pico do `ru_maxrss` e explicar a diferença; (4) desenhar o caminho Python → objeto → memória para um único `m[i][j]`.

**Checklist:** igualdade conferida antes de medir; as duas representações interpretadas separadamente; pico ≠ atual sem hesitar; página lida do SO, não decorada.

---

### Como esta aula será avaliada

| Critério | Peso |
| -------- | ---: |
| Conceitos e vocabulário (nível 1) | 20% |
| Previsão de comportamento e diagnóstico de erros (nível 2) | 40% |
| Projeto e justificativa de decisões (nível 3) | 30% |
| Registro de comandos, saídas e interpretação | 10% |

**Aprovação:** nota mínima 7,0; pelo menos 60% dos pontos devem vir de previsão e diagnóstico (níveis 2 e 3), não apenas de código que executa.

**Para se aprofundar:**

1. *High Performance Python*, Gorelick e Ozsvald (O'Reilly, ISBN 978-1492055020); 2. Documentação de `threading`/`asyncio` em [docs.python.org/3/library](https://docs.python.org/3/library/); 3. Luciano Ramalho, *Python Fluente*, caps. sobre concorrência.
