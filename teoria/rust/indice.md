---
layout: default
---

# Indice Geral da Trilha de Rust

---

## Visao Geral

Este arquivo organiza o material existente de Rust, ainda em revisão curricular e técnica, cobrindo:

- prompt-base da trilha;
- fase principal de [`01.md`](01.md) a [`18.md`](18.md);
- plano de especializacao;
- fase estendida de [`20.md`](20.md) a [`31.md`](31.md);
- extensao opcional em [`32_bonus.md`](32_bonus.md).

Use este indice como ponto de entrada rapido para estudo, revisao e navegacao entre os modulos.

---

## Arquivos Base

- [Mapa SDD do livro](MAPA_SDD.md): competências, pré-requisitos, lacunas e avaliações necessárias. O material atual está em revisão e ainda não passou pelo gate editorial.
- `prompt.md`: prompt-base da trilha de Rust
- [`19_plano_estudos.md`](19_plano_estudos.md): plano de estudos da fase avancada

---

## Trilha Principal

### Fundamentos da Linguagem

1. [`01.md`](01.md) - Introducao ao Rust, `rustc`, `cargo`, estrutura de um programa, `fn main`, `println!`, variaveis, mutabilidade e tipos basicos
2. [`02.md`](02.md) - Controle de fluxo, funcoes, expressoes vs statements
3. [`03.md`](03.md) - Ownership, borrowing e references
4. [`04.md`](04.md) - Slices, strings e arrays
5. [`05.md`](05.md) - Structs, enums e pattern matching
6. [`06.md`](06.md) - Modulos, organizacao de arquivos e crates
7. [`07.md`](07.md) - Collections
8. [`08.md`](08.md) - Tratamento de erros com `Result` e `Option`
9. [`09.md`](09.md) - Generics, traits e bounds
10. [`10.md`](10.md) - Lifetimes

### Engenharia de Software e Sistemas

11. [`11.md`](11.md) - Iterators e closures
12. [`12.md`](12.md) - Smart pointers
- [`12a_raciocinio_e_algoritmos.md`](12a_raciocinio_e_algoritmos.md) - Aula de ponte: invariantes, inducao, busca binaria, BFS, hash e custo antes da concorrencia
- [`12b_arvores_e_ordenacao.md`](12b_arvores_e_ordenacao.md) - Aula de ponte (R3): BST com `Box`, degeneracao medida e `sort_unstable` como oraculo
13. [`13.md`](13.md) - Concorrencia e paralelismo em Rust
14. [`14.md`](14.md) - Async e runtime fundamentals
- [`14a_framing_e_concorrencia.md`](14a_framing_e_concorrencia.md) - Aula de ponte: framing TCP, limites, timeouts e concorrencia com `std`
- [`14b_ordem_e_backpressure.md`](14b_ordem_e_backpressure.md) - Aula de ponte (R5): Release/Acquire medido, canal limitado com backpressure e cancelamento cooperativo
- [`14c_cercas_e_seqlock.md`](14c_cercas_e_seqlock.md) - Aula de ponte (R5): fence publica par sem operacao ordenada, seqlock com leitor otimista e retentativa
15. [`15.md`](15.md) - Unsafe Rust
- [`15a_auditoria_unsafe.md`](15a_auditoria_unsafe.md) - Aula de ponte: auditoria de `unsafe` por invariante escrita, com teste por bloco
16. [`16.md`](16.md) - FFI com C/C++
- [`16a_maquina_e_so.md`](16a_maquina_e_so.md) - Aula de ponte: processos, syscalls, `/proc`, RSS e localidade em Rust seguro
- [`16b_maquina_e_cache.md`](16b_maquina_e_cache.md) - Aula de ponte (R4): localidade linhas/colunas medida, custo de syscall e pagina via /proc
- [`16c_isa_e_abi.md`](16c_isa_e_abi.md) - Aula de ponte (R4): ISA nomeada, layout `repr(C)` contratado, `extern "C"` por nome e ponteiro
17. [`17.md`](17.md) - Performance tuning e profiling
18. [`18.md`](18.md) - Boas praticas para sistemas de alto desempenho e codigo de producao

---

## Plano de Especializacao

- [`19_plano_estudos.md`](19_plano_estudos.md) organiza a fase avancada em modulos de:
  - backend de alta performance e sistemas distribuidos
  - computacao de alto desempenho, GPU e SIMD
  - programacao de sistemas e baixo nivel
  - projetos praticos

---

## Trilha Estendida

### Modulo 1 - Backend de Alta Performance e Sistemas Distribuidos

20. [`20.md`](20.md) - Servidores HTTP de alta performance: anatomia do `hyper` e `axum`
21. [`21.md`](21.md) - WebSockets e arquitetura orientada a eventos
22. [`22.md`](22.md) - Bancos de dados de alta performance: `sqlx` e zero-copy deserialization
23. [`23.md`](23.md) - Comunicacao entre microservicos: gRPC, Protobuf e `tonic`
- [`23a_hpc_baseline.md`](23a_hpc_baseline.md) - Aula de ponte: baseline honesto, multicore com `scope`, contenção granular e conta da GPU
- [`23b_sistemas_distribuidos.md`](23b_sistemas_distribuidos.md) - Aula de ponte (R6): dois nós com log e `sync_all`, idempotência por id, kill, cauda rasgada, replay e atraso
- [`23d_quorum_e_reparo.md`](23d_quorum_e_reparo.md) - Aula de ponte (R6): quorum de escrita/leitura com vetor, conflito declarado e reparo em duas passadas
- [`23c_vetorizacao.md`](23c_vetorizacao.md) - Aula de ponte (R7): soma com quatro acumuladores contra referencia f64, razao medida e NUMA contado
- [`23e_afinidade_e_numa.md`](23e_afinidade_e_numa.md) - Aula de ponte (R7): triade com primeiro toque vs tardio, afinidade por `taskset` e laboratorio multino declarado

### Modulo 2 - Computacao de Alto Desempenho, GPU e SIMD

24. [`24.md`](24.md) - HPC: instrucoes SIMD nativas (`std::simd`) e otimizacao extrema
25. [`25.md`](25.md) - Introducao a GPGPU: `wgpu` e compute shaders
26. [`26.md`](26.md) - Ecossistema CUDA e compilando Rust para a GPU (`rust-gpu`)

### Modulo 3 - Programacao de Sistemas e Baixo Nivel

27. [`27.md`](27.md) - Bare metal: Rust `#![no_std]` e sistemas embarcados
28. [`28.md`](28.md) - Interagindo com o kernel: syscalls diretas e `mmap`
29. [`29.md`](29.md) - O fundo do poco: programacao eBPF no kernel Linux (`Aya`)

### Modulo 4 - Projetos Praticos

30. [`30.md`](30.md) - Masterpiece A: construindo um load balancer TCP do zero
31. [`31.md`](31.md) - Masterpiece B: banco de dados in-memory concorrente (estilo Redis)

### Extensao Opcional

32. [`32_bonus.md`](32_bonus.md) - Masterpiece B avancado: zero-copy, `INCR` e expiracao TTL

---

## Sugestoes de Leitura

### Caminho Linear

Se voce quiser seguir a trilha como curso:

1. leia [`01.md`](01.md) a [`18.md`](18.md), incluindo a aula de ponte [`12a_raciocinio_e_algoritmos.md`](12a_raciocinio_e_algoritmos.md) depois de [`12.md`](12.md), a aula [`14a_framing_e_concorrencia.md`](14a_framing_e_concorrencia.md) depois de [`14.md`](14.md) e a aula [`16a_maquina_e_so.md`](16a_maquina_e_so.md) depois de [`16.md`](16.md)
2. leia [`19_plano_estudos.md`](19_plano_estudos.md)
3. siga [`20.md`](20.md) a [`31.md`](31.md), incluindo a aula [`23a_hpc_baseline.md`](23a_hpc_baseline.md) depois de [`23.md`](23.md)
4. avance para [`32_bonus.md`](32_bonus.md) se quiser fechar a extensao opcional

### Caminho por Objetivo

Se o foco for linguagem e base:

- [`01.md`](01.md) a [`12.md`](12.md)

Se o foco for concorrencia, async e backend:

- [`13.md`](13.md)
- [`14.md`](14.md)
- [`20.md`](20.md) a [`23.md`](23.md)
- [`30.md`](30.md)

Se o foco for HPC, GPU e SIMD:

- [`17.md`](17.md)
- [`24.md`](24.md) a [`26.md`](26.md)

Se o foco for baixo nivel e sistema:

- [`15.md`](15.md)
- [`16.md`](16.md)
- [`27.md`](27.md) a [`29.md`](29.md)

Se o foco for projetos praticos:

- [`30.md`](30.md)
- [`31.md`](31.md)
- [`32_bonus.md`](32_bonus.md)

---

## Fechamento

O conjunto atual introduz Rust do zero e avança até temas de engenharia de sistemas, com ênfase em:

- ownership e borrowing;
- concorrencia e async;
- FFI e unsafe com criterio;
- backend de alta performance;
- HPC, SIMD e GPU;
- syscalls, `mmap` e eBPF;
- projetos praticos de alto nivel tecnico.

O [mapa SDD](MAPA_SDD.md) identifica as competências, lacunas e avaliações necessárias antes que este material possa ser tratado como livro concluído.

## Kernel (percurso R8)

- [`28a_kernel_riscv.md`](28a_kernel_riscv.md) - Kernel RISC-V em Rust `no_std` (`rust/kernel/`): os sete marcadores das trilhas C/C++ so com `core`
- [`28b_x86_64_comparativo.md`](28b_x86_64_comparativo.md) - Comparativo x86-64 executado (`rust/kernel-x86/`) e a tabela do que nao porta
