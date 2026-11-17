---
layout: default
---

# Indice Geral da Trilha de Python

> **Estado editorial:** livro em revisão SDD. Consulte o [mapa de competências e lacunas](MAPA_SDD.md) antes de usar a trilha como curso completo.

---

## Visao Geral

Este arquivo organiza a trilha completa de Python em `d:\TI\git\Algoritmos\teoria\python`, cobrindo:

- prompt-base da trilha;
- fase principal de `01.md` a `19.md`;
- plano de especializacao;
- fase estendida de `20.md` a `32.md`.

Use este indice como ponto de entrada rapido para estudo, revisao e navegacao entre os modulos.

---

## Arquivos Base

- `prompt.md`: prompt-base da trilha de Python
- `19_plano_estudos.md`: plano de estudos da fase avancada

---

## Trilha Principal

### Fundamentos da Linguagem

1. `01.md` - Introducao ao Python, interpretador, estrutura de um programa, `print()`, variaveis, tipos basicos e execucao de scripts
2. `02.md` - Controle de fluxo, operadores, funcoes e escopo de execucao
3. `03.md` - Modelo de objetos, referencias, mutabilidade, identidade e escopo
4. `04.md` - Strings, listas, tuplas, dicionarios e conjuntos
- [`04a_raciocinio_e_algoritmos.md`](04a_raciocinio_e_algoritmos.md) - Aula de ponte: invariantes, inducao, busca binaria, grafos, hash e custo antes de estudar objetos em profundidade
- [`04b_arvores_e_ordenacao.md`](04b_arvores_e_ordenacao.md) - Aula de ponte (PY2): BST em Python puro, degeneracao medida e por que a std vence
5. `05.md` - Funcoes avancadas, argumentos, closures e decorators
6. `06.md` - Modulos, imports, pacotes e organizacao de projetos
7. `07.md` - Orientacao a objetos em Python: classes, instancias, atributos e heranca
8. `08.md` - Tratamento de erros e excecoes em Python
9. `09.md` - Iteradores, generators e comprehensions
10. `10.md` - Context managers e protocolo de objetos

### Engenharia de Software em Python

11. `11.md` - Tipagem com `type hints` em Python
12. `12.md` - `dataclasses`, `enum` e padroes idiomaticos
13. `13.md` - Arquivos, serializacao e I/O
14. `14.md` - Testes, debug e qualidade de codigo
- [`14a_do_interpretador_ao_kernel.md`](14a_do_interpretador_ao_kernel.md) - Aula de ponte: bytecode, CPython, RSS contra tracemalloc, syscalls e SO pelo espaco de usuario
- [`14b_cache_e_pagina.md`](14b_cache_e_pagina.md) - Aula de ponte (PY4): localidade em duas representacoes, pagina e pico de RSS do SO
15. `15.md` - Concorrencia com `threading` e `multiprocessing`
16. `16.md` - `async`/`await` e fundamentos de `asyncio`
- [`16a_concorrencia_e_gil.md`](16a_concorrencia_e_gil.md) - Aula de ponte: GIL por versao, corrida demonstrada, threads contra processos e `asyncio`
- [`16b_deadlock_e_backpressure.md`](16b_deadlock_e_backpressure.md) - Aula de ponte (PY5): deadlock coreografado e detectado, ordem global e fila com teto
- [`19a_protocolos_tcp.md`](19a_protocolos_tcp.md) - Aula de ponte (PY6): framing TCP puro com tetos, antes de framework
17. `17.md` - Performance tuning, profiling e uso eficiente de memoria
18. `18.md` - Integracao com C/C++, NumPy e bases para HPC
19. `19.md` - Boas praticas para backend, automacao e sistemas de producao

---

## Plano de Especializacao

- `19_plano_estudos.md` organiza a fase avancada em modulos de:
  - backend, APIs e servicos de producao
  - automacao, CLIs e integracao com infraestrutura
  - dados, NumPy, performance e fronteira nativa
  - projetos praticos de engenharia em Python

---

## Trilha Estendida

### Modulo 1 - Backend, APIs e Servicos de Producao

20. `20.md` - APIs Web com FastAPI/ASGI e organizacao em camadas
21. `21.md` - Banco de dados, ORMs, pooling e transacoes
22. `22.md` - Filas, workers, retries e idempotencia
23. `23.md` - Observabilidade e operacao de servicos Python
- [`23a_sistemas_distribuidos.md`](23a_sistemas_distribuidos.md) - Aula de ponte: falhas parciais, idempotencia, log duravel e recuperacao com dois nos locais
- [`23b_relogios_e_particao.md`](23b_relogios_e_particao.md) - Aula de ponte (PY7): Lamport, vetores, particao bilateral e politica de consistencia declarada

### Modulo 2 - Automacao, CLIs e Integracao com Infraestrutura

24. `24.md` - CLIs profissionais, configuracao e codigos de saida
25. `25.md` - Subprocessos, SSH, APIs externas e automacao resiliente
26. `26.md` - Agendamento, jobs, integracao com arquivos e processos repetiveis

### Modulo 3 - Dados, NumPy, Performance e Fronteira Nativa

27. `27.md` - NumPy profundo, layout de memoria e vetorizacao
28. `28.md` - Pandas, ETL e processamento de dados com criterio
29. `29.md` - Profiling avancado, memoria e integracao com Cython, Numba e FFI
- [`29a_hpc_e_modelo_gpu.md`](29a_hpc_e_modelo_gpu.md) - Aula de ponte: baseline escalar, NumPy, threads, processos e a conta da viagem para GPU
- [`29b_seguranca_defensiva.md`](29b_seguranca_defensiva.md) - Aula de ponte (PY9): pickle recusado, JSON com esquema, subprocess sem shell
- [`29c_localidade_e_amdahl.md`](29c_localidade_e_amdahl.md) - Aula de ponte (PY8): processos medidos, serializacao como viagem e tetos de Amdahl

### Modulo 4 - Projetos Praticos

30. `30.md` - Projeto pratico: construindo um servico backend em Python
31. `31.md` - Projeto pratico: ferramenta de automacao de infraestrutura em Python
32. `32.md` - Projeto pratico: pipeline de dados com Python, NumPy e Pandas
- [`33_projeto_integrador.md`](33_projeto_integrador.md) - Projeto integrador (PY10): servico replicado + CLI de SO + carga, sob rubrica

---

## Sugestoes de Leitura

### Caminho Linear

Se voce quiser seguir a trilha como curso:

1. leia `01.md` a `19.md`, incluindo a aula de ponte `04a_raciocinio_e_algoritmos.md` depois de `04.md`, a aula `14a_do_interpretador_ao_kernel.md` depois de `14.md` e a aula `16a_concorrencia_e_gil.md` depois de `16.md`
2. leia `19_plano_estudos.md`
3. siga `20.md` a `32.md`, incluindo a aula `23a_sistemas_distribuidos.md` depois de `23.md` e a aula `29a_hpc_e_modelo_gpu.md` depois de `29.md`

### Caminho por Objetivo

Se o foco for linguagem e base:

- `01.md` a `12.md`

Se o foco for backend e servicos:

- `19.md`
- `20.md` a `23.md`
- `30.md`

Se o foco for automacao e ferramentas internas:

- `13.md`
- `19.md`
- `24.md` a `26.md`
- `31.md`

Se o foco for dados, NumPy e performance:

- `17.md`
- `18.md`
- `27.md` a `29.md`
- `32.md`

Se o foco for concorrencia e assincronia:

- `15.md`
- `16.md`
- `22.md`
- `26.md`

---

## Fechamento

O conjunto atual cobre uma trilha de Python do zero ate uma fase estendida de engenharia aplicada, com enfase em:

- modelo de objetos;
- mutabilidade e referencias;
- tipagem e organizacao de codigo;
- backend e automacao;
- concorrencia e assincronia;
- dados, NumPy e ETL;
- observabilidade e operacao;
- profiling e integracao nativa;
- projetos praticos.

Este indice serve como mapa geral da trilha.
