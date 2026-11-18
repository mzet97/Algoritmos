---
layout: default
---

# Indice Geral da Trilha de C

> **Estado editorial:** livro em revisão SDD. Consulte o [mapa de competências e lacunas](MAPA_SDD.md) antes de usar a trilha como curso completo. As aulas 01, 04, 07, 14, 23 e 31 formam o piloto; os demais capítulos ainda não passaram pelo gate de publicação.

---

## Visao Geral

Este arquivo organiza a trilha completa de C em `d:\TI\git\Algoritmos\teoria\c`, cobrindo:

- prompt-base da trilha;
- fase principal de `01.md` a `19.md`;
- plano de especializacao;
- fase estendida de `20.md` a `32.md`.

Use este indice como ponto de entrada rapido para estudo, revisao e navegacao entre os modulos.

---

## Arquivos Base

- `prompt.md`: prompt-base da trilha de C
- `19_plano_estudos.md`: plano de estudos da fase avancada

---

## Trilha Principal

### Fundamentos da Linguagem

1. [`01.md`](01.md) - Introducao a C, compilacao com `gcc` e `clang`, `main`, `printf`, estrutura de um programa e toolchain basica
2. `02.md` - Tipos basicos, variaveis, constantes, operadores e controle de fluxo
3. `03.md` - Funcoes, prototipos, escopo, stack frame e organizacao basica do codigo
- [`03a_raciocinio_algoritmico.md`](03a_raciocinio_algoritmico.md) - Aula de ponte: invariantes, prova de correcao e custo antes de estudar arrays e ponteiros
4. [`04.md`](04.md) - Arrays, strings em C, ponteiros e aritmetica de ponteiros
5. `05.md` - `struct`, `union`, `enum` e layout de memoria
6. `06.md` - Headers, multiplos arquivos, preprocessor, macros e organizacao de projetos
7. [`07.md`](07.md) - Gerenciamento manual de memoria com `malloc`, `calloc`, `realloc` e `free`
8. `08.md` - Entrada e saida com `stdio.h`, arquivos, buffers e erros
9. `09.md` - Function pointers, callbacks e tabelas de despacho
10. `10.md` - Undefined behavior, aliasing, alignment, padding e armadilhas classicas
- [`10a_arquitetura_e_memoria_virtual.md`](10a_arquitetura_e_memoria_virtual.md) - Aula de ponte: ISA, processo, memoria virtual, paginas, TLB, cache e mapa de memoria

### Engenharia de Software Nativo

11. `11.md` - Depuracao com `gdb`, sanitizers e `valgrind`
12. `12.md` - Bibliotecas estaticas, dinamicas, linkagem e ABI
13. `13.md` - Estruturas de dados classicas em C
- [`13a_algoritmos_e_estruturas.md`](13a_algoritmos_e_estruturas.md) - Aula de ponte: busca binaria, hash com sondagem, qsort como oraculo e conta de alocacao verificada
- [`13b_arvores_e_ordenacao.md`](13b_arvores_e_ordenacao.md) - Aula de ponte: BST com tres casos de remocao, degeneracao medida (altura 1000 vs 24) e mergesort estavel contra qsort
14. [`14.md`](14.md) - Concorrencia com `pthreads`, mutexes, condvars e atomics
- [`14a_concorrencia_e_threads.md`](14a_concorrencia_e_threads.md) - Aula de ponte: modelo de memoria C11, data race, ciclo de vida e condvar com predicado
15. `15.md` - Sockets, rede e programacao de sistemas Unix-like
16. `16.md` - Syscalls, `mmap`, arquivos grandes e I/O de baixo nivel
- [`16a_processos_e_observacao.md`](16a_processos_e_observacao.md) - Aula de ponte: fork/wait, régua de syscall, `/proc/self/maps`, faltas de página e exec contido
17. `17.md` - Performance tuning, cache, profiling e otimizacao real
18. `18.md` - Interoperabilidade com C++, Rust e outras linguagens
19. `19.md` - Boas praticas para bibliotecas, CLIs, backend e sistemas de producao em C

---

## Plano de Especializacao

- `19_plano_estudos.md` organiza a fase avancada em modulos de:
  - rede e IPC
  - heap, allocators e buffers
  - performance profunda e hardware
  - projetos praticos

---

## Trilha Estendida

### Modulo 1 - Rede, IPC e Servidores

20. `20.md` - `select`, `poll`, `epoll` e multiplexacao de I/O
21. `21.md` - Modelos de servidor: thread por conexao vs event-driven
22. `22.md` - IPC com pipes, Unix domain sockets e memoria compartilhada
23. [`23.md`](23.md) - Framing de protocolo, parsing incremental e robustez de entrada
- [`23a_sistemas_distribuidos.md`](23a_sistemas_distribuidos.md) - Aula de ponte: dois nós chave-valor, log com `fsync`, idempotência por `id`, `SIGKILL`, replay, cauda rasgada e atraso injetado
- [`23b_fuzzing_e_limites.md`](23b_fuzzing_e_limites.md) - Aula de ponte: fuzzing determinístico (200 mil mutações, semente fixa) do parser da 23a, tetos e alfabeto sob ASan/UBSan
- [`23c_relogios_e_particao.md`](23c_relogios_e_particao.md) - Aula de ponte: relógios de Lamport e vetoriais, concorrência detectada, partição bilateral reconciliada com perda declarada
- [`23d_consenso_e_quorum.md`](23d_consenso_e_quorum.md) - Aula de ponte: eleição por maioria, commit por maioria, minoria que não confirma e cura com termo novo

### Modulo 2 - Heap, Alocacao e Infraestrutura Manual

24. `24.md` - Heap real, fragmentacao e comportamento do allocator
25. `25.md` - Arenas, pools e estrategias customizadas de alocacao
26. `26.md` - Strings, buffers e builders eficientes em C
27. `27.md` - Mini runtime de containers e APIs reutilizaveis

### Modulo 3 - Performance Profunda e Hardware

28. `28.md` - Cache lines, prefetch, branch prediction e false sharing em profundidade
- [`28a_benchmark_honesto.md`](28a_benchmark_honesto.md) - Aula de ponte: protocolo de medicao, escala limitada por banda, falso compartilhamento e conta do acelerador
29. `29.md` - Profiling de CPU e memoria com `perf`, `callgrind` e counters
30. `30.md` - SIMD, vetorizacao e criterio para otimizar loops criticos
- [`30a_modelo_gpu.md`](30a_modelo_gpu.md) - Aula de ponte: SIMT, hierarquia de memoria, conta da transferencia e referencia em CPU
- [`30b_simd_e_afinidade.md`](30b_simd_e_afinidade.md) - Aula de ponte: SIMD verificado contra oraculo em double, dispatch AVX2 em tempo de execucao, afinidade confirmada e NUMA contado

### Modulo 4 - Projetos Praticos

31. [`31.md`](31.md) - Projeto pratico de servidor TCP/event-driven em C
32. `32.md` - Projeto pratico de biblioteca/CLI de infraestrutura em C
- [`32a_kernel_riscv.md`](32a_kernel_riscv.md) - Aula de ponte/projeto: kernel RISC-V em QEMU (boot, PMP, Sv39, traps, syscall, falta sob demanda, escalonador preemptivo, 2 harts) em `kernel/`, com teste automatizado de 7 marcadores
- [`32b_x86_64_comparativo.md`](32b_x86_64_comparativo.md) - Aula comparativa: Multiboot, modo longo e IDT em x86-64 (`kernel-x86/`), com tabela de correspondências RISC-V x x86-64

---

## Sugestoes de Leitura

### Caminho Linear

Se voce quiser seguir a trilha como curso:

1. leia `01.md` a `19.md`, incluindo as pontes `03a_raciocinio_algoritmico.md` (apos `03.md`), `10a_arquitetura_e_memoria_virtual.md` (apos `10.md`), `13a_algoritmos_e_estruturas.md` (apos `13.md`), `14a_concorrencia_e_threads.md` (apos `14.md`) e `16a_processos_e_observacao.md` (apos `16.md`)
2. leia `19_plano_estudos.md`
3. siga `20.md` a `32.md`

### Caminho por Objetivo

Se o foco for linguagem e base:

- `01.md` a `12.md`

Se o foco for debugging, sistemas e rede:

- `11.md` a `16.md`

Se o foco for backend nativo, multiplexacao e protocolos:

- `20.md` a `23.md`
- `31.md`

Se o foco for memoria manual, allocators e bibliotecas:

- `07.md`
- `24.md` a `27.md`
- `32.md`

Se o foco for performance real:

- `17.md`
- `28.md`
- [`28a_benchmark_honesto.md`](28a_benchmark_honesto.md)
- `29.md`
- `30.md`
- [`30a_modelo_gpu.md`](30a_modelo_gpu.md)

---

## Fechamento

O conjunto atual cobre uma trilha de C do zero ate uma fase estendida de engenharia nativa, com enfase em:

- memoria manual;
- ownership explicito;
- ABI e linkagem;
- concorrencia;
- sistemas e rede;
- observabilidade e diagnostico;
- profiling;
- custo real de hardware;
- projetos praticos.

Este indice serve como mapa geral da trilha.
