---
layout: default
---

# Indice Geral da Trilha de C++

---

## Visao Geral

Este arquivo organiza a trilha completa de C++ em `d:\TI\git\Algoritmos\teoria\cpp`, cobrindo:

- prompt-base da trilha;
- fase principal de `01.md` a `19.md`;
- plano de especializacao;
- fase estendida de `20.md` a `32.md`.

Use este indice como ponto de entrada rapido para estudo, revisao e navegacao entre os modulos.

O [mapa SDD do livro](MAPA_SDD.md) registra competencias, evidencias de
aprendizagem, modulos de ponte e projetos ainda necessarios. **Estado do volume:
rascunho.** As aulas 01–32 formam a base existente; o mapa distingue essa base
do percurso completo ate sistemas distribuidos, HPC/GPU e kernel didatico.

Os exemplos usam C++20 como base, salvo indicacao explicita. A Aula 11
requer C++23 e uma biblioteca padrao com `<expected>`. A Aula 31 e um
esboco local, bloqueante, de servidor com `poll` para Linux; a conversao
para I/O nao bloqueante faz parte da etapa de aprofundamento.

---

## Arquivos Base

- `prompt.md`: prompt-base da trilha de C++
- `19_plano_estudos.md`: plano de estudos da fase avancada

---

## Trilha Principal

### Fundamentos da Linguagem

1. `01.md` - Introducao a C++, compilacao com `g++` e `clang++`, `main`, `std::cout`, estrutura de um programa e toolchain basica
2. `02.md` - Tipos basicos, `const`, referencias, operadores e controle de fluxo
3. `03.md` - Funcoes, sobrecarga, namespaces, escopo e organizacao basica do codigo
- [`03a.md`](03a.md) - Aula de ponte: logica, conjuntos, invariantes, inducao, recursao e limite da abstracao de maquina
4. `04.md` - Classes, `struct`, encapsulamento, construtores, destrutores e RAII
5. `05.md` - Copy semantics, move semantics, regra dos 3, 5 e 0
6. `06.md` - Ponteiros, referencias, `new`, `delete`, smart pointers e ownership
7. `07.md` - Templates, genericidade, `auto`, deducao de tipos e `concepts`
8. `08.md` - `std::string`, containers da STL, iteradores e algoritmos
- [`08a.md`](08a.md) - Aula de ponte: busca binaria, insercao e mergesort a mao contra `std::sort`/`binary_search` como oraculos, medidos em tres tamanhos
9. `09.md` - Lambdas, functors, `std::function`, callbacks e callable objects
10. `10.md` - Polimorfismo, heranca, `virtual`, interfaces e object model de C++

### Engenharia de Software Nativo

11. `11.md` - Tratamento de erros com excecoes, `std::optional`, `std::variant` e `std::expected`
12. `12.md` - Headers, multiplos arquivos, templates em headers, ODR, linkagem e ABI
- [`12a.md`](12a.md) - Aula de ponte: ISA, ABI, modo usuario/kernel, syscalls, `/proc` e faltas de pagina
13. `13.md` - Depuracao com `gdb`, sanitizers e `valgrind`
- [`13a.md`](13a.md) - Aula de ponte: overflow, use-after-free e double-free reproduzidos sob ASan, com `at()`, `unique_ptr`, conta verificada e copia com teto
- [`13b_ffi_e_ameacas.md`](13b_ffi_e_ameacas.md) - Aula de ponte: fronteira `extern "C"` com `noexcept`, dono unico opaco, abuso virando codigo e modelo de ameacas
14. `14.md` - Concorrencia com `std::thread`, mutexes, condvars, atomics e memory model
15. `15.md` - Alocadores, layout, cache, profiling e otimizacao real
16. `16.md` - `filesystem`, streams, parsing e CLIs em C++
17. `17.md` - Interoperabilidade com C, Rust e outras linguagens
- [`17a.md`](17a.md) - Aula de ponte: sinais, `mutex` contra `atomic`, TSan e escalonamento visto do usuario
18. `18.md` - Redes e programacao de sistemas com C++ sobre APIs nativas
19. `19.md` - Boas praticas para bibliotecas, backend, ferramentas e sistemas de producao em C++

---

## Plano de Especializacao

- `19_plano_estudos.md` organiza a fase avancada em modulos de:
  - assincronia e rede
  - memoria, ABI e bibliotecas
  - performance profunda e hardware
  - projetos praticos

---

## Trilha Estendida

### Modulo 1 - Assincronia, Rede e Execucao

20. `20.md` - Event loops, reactor, proactor e o lugar de C++ em I/O assincrono
21. `21.md` - `std::future`, `std::promise`, thread pools e composicao de tarefas
22. `22.md` - Coroutines em C++ com `co_await`, `co_return`, frame e custo real
23. `23.md` - Servidores modernos em C++ com buffers, protocolo, backpressure e arquitetura orientada a eventos
- [`23a.md`](23a.md) - Aula de ponte: servidor nao bloqueante com `EAGAIN`, timeout de ociosidade, teto por conexao e teste de adversarios
- [`23b.md`](23b.md) - Aula de ponte: dois nos replicados com log duravel, idempotencia, `SIGKILL` real e teste de queda/reinicio/atraso
- [`23c.md`](23c.md) - Aula de ponte: relogios vetoriais, leitura por quorum, token entre nos e consistencia alem do primario-replica
- [`23d_limites_globais.md`](23d_limites_globais.md) - Aula de ponte: tetos globais do servidor (conexoes, por IP, bytes por janela) sobre a base da 23a
- [`23e_fuzzing_sistematico.md`](23e_fuzzing_sistematico.md) - Aula de ponte: fuzzing sistematico do parser de framing com mutador deterministico sob sanitizers
- [`23f_eleicao_commit_rpc.md`](23f_eleicao_commit_rpc.md) - Aula de ponte: eleicao por maioria, commit por maioria, cura com descarte nomeado e RPC idempotente

### Modulo 2 - Memoria, ABI e Infraestrutura de Biblioteca

24. `24.md` - `std::pmr`, `memory_resource` e estrategias de alocacao em C++ moderno
25. `25.md` - Type erasure, `std::function`, SBO e custo de abstracoes de chamada
26. `26.md` - PIMPL, estabilidade binaria, headers mais leves e contratos de biblioteca
27. `27.md` - Instrumentation, observabilidade nativa, logs, tracing e metricas em C++

### Modulo 3 - Performance Profunda e Hardware

28. `28.md` - Cache lines, layout, false sharing e performance multicore em C++
29. `29.md` - Profiling de CPU e memoria com `perf`, `callgrind` e counters em C++
30. `30.md` - SIMD, vetorizacao, `-march`, auto-vectorization e criterio para otimizar loops criticos em C++
- [`30a.md`](30a.md) - Aula de ponte: matvec multicore por linhas, ganho medido e o caso em que paralelizar perde
- [`30b.md`](30b.md) - Aula de ponte: referencia CPU de SAXPY, conta da viagem PCIe e o limiar onde a GPU perderia

### Modulo 4 - Projetos Praticos

31. `31.md` - Esboco didatico de servidor TCP com `poll` em C++
32. `32.md` - Projeto pratico de biblioteca/CLI de infraestrutura em C++

---

## Sugestoes de Leitura

### Caminho Linear

Se voce quiser seguir a trilha como curso:

1. leia `01.md` a `19.md`, incluindo a aula de ponte `03a.md` depois de `03.md`, a aula `12a.md` depois de `12.md` e a aula `17a.md` depois de `17.md`; na extensao, leia as aulas `23a.md` e `23b.md` depois de `23.md`
2. leia `19_plano_estudos.md`
3. siga `20.md` a `32.md`

### Caminho por Objetivo

Se o foco for linguagem e base:

- `01.md` a `12.md`

Se o foco for debugging, concorrencia e sistemas:

- `13.md` a `18.md`

Se o foco for backend nativo, event loop e rede:

- `20.md` a `23.md`
- `31.md`

Se o foco for memoria, bibliotecas e contratos:

- `24.md` a `27.md`
- `32.md`

Se o foco for performance real:

- `15.md`
- `28.md`
- `29.md`
- `30.md`

---

## Fechamento

O conjunto atual cobre uma trilha de C++ do zero ate uma fase estendida de engenharia nativa, com enfase em:

- ownership;
- RAII;
- contratos de API;
- concorrencia;
- sistemas;
- observabilidade;
- profiling;
- custo real de hardware;
- projetos praticos.

Este indice serve como mapa geral da trilha.

## Kernel (percurso CPP-12/CPP-13)

- [`33.md`](33.md) - Kernel didatico RISC-V em C++ freestanding (`cpp/kernel/`): os sete marcadores da trilha de C, compilados como C++20 sem excecoes/RTTI
- [`40.md`](40.md) - Comparacao x86-64 executada (`cpp/kernel-x86/`): Multiboot, modo longo, IDT e a tabela do que nao porta (PMP)
