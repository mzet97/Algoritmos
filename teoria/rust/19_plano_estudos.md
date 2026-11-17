# Aula 19 — Roadmap Avançado: Especialização em Sistemas e Alta Performance

## Objetivo da Aula
Após concluirmos a fundação rigorosa de Rust (Aulas 1 a 18), equivalente ao domínio avançado de C e C++ moderno, este documento estabelece o nosso **Plano de Estudos de Especialização**.

Daqui para a frente, abandonamos os fundamentos da linguagem e passamos a focar exclusivamente em domínios de aplicação reais, extremos e de baixo nível. As próximas aulas serão estruturadas nos quatro pilares abaixo.

Você pode optar por seguir sequencialmente ou me indicar qual módulo deseja atacar primeiro.

Esta aula não introduz um recurso novo da linguagem. Ela organiza a segunda metade da trilha em termos de engenharia aplicada.

Mapa mental:

- até a Aula 18, o foco foi formar base sólida da linguagem e do modelo mental de Rust;
- a partir daqui, o foco passa a ser **domínio de aplicação**;
- cada módulo representa uma família real de problemas de engenharia.

---

## Módulo 1: Backend de Alta Performance e Sistemas Distribuídos
*Foco: I/O Bound extremo, suportar milhões de conexões concorrentes, uso mínimo de RAM e processamento zero-copy.*

- **Backend de alta performance**: software de servidor otimizado para throughput, latência baixa e uso eficiente de recursos.
- **Sistemas distribuídos**: sistemas em que múltiplos processos ou serviços cooperam através de rede.
- **I/O Bound**: carga dominada por espera de rede, disco, banco de dados ou socket, não por cálculo puro.
- **Zero-copy**: estratégia de evitar cópias desnecessárias de dados entre buffers e camadas de software.

*   **Aula 20:** **Servidores HTTP de Alta Performance** — Anatomia do `hyper` e `axum`. Como lidar com roteamento *zero-copy* e compartilhar estado global de forma segura sem travar o Event Loop com *lock contention*.
*   **Aula 21:** **WebSockets e Arquitetura Orientada a Eventos** — Como manter centenas de milhares de conexões abertas usando pouca memória. Passagem de mensagens assíncronas entre clients.
*   **Aula 22:** **Bancos de Dados com Zero-Copy Deserialization** — Uso de `sqlx` e `serde` para mapear resultados do banco diretamente para structs Rust usando *borrowing* estrito (retornando referências para o buffer de rede, sem alocar `String`s).
*   **Aula 23:** **Comunicação entre Microserviços (gRPC)** — Serialização binária com Protobufs e construção de servidores gRPC ultra-rápidos usando a crate `tonic`.

---

## Módulo 2: Computação de Alto Desempenho (HPC), GPU e SIMD
*Foco: CPU Bound extremo, cálculos matemáticos, processamento paralelo em nível de hardware.*

- **HPC (High-Performance Computing)**: computação voltada a maximizar desempenho numérico e throughput em hardware especializado ou fortemente otimizado.
- **CPU Bound**: carga dominada por cálculo, não por espera de I/O.
- **SIMD**: uma instrução operando sobre múltiplos dados de uma vez.
- **GPU**: processador especializado em paralelismo massivo de dados.

*   **Aula 24:** **Instruções SIMD Nativas** — Otimizando loops em nível de assembly (AVX2/AVX-512) usando `std::simd` portátil, processando múltiplos dados em um único ciclo de clock (o substituto moderno das *intrinsics* de C).
*   **Aula 25:** **Introdução a GPGPU (wgpu)** — Migrando cálculos massivamente paralelos da CPU para a Placa de Vídeo. Como escrever *Compute Shaders* modernos usando WebGPU/Vulkan sob o capô do Rust.
*   **Aula 26:** **Ecossistema CUDA e rust-gpu** — Explorando a fronteira do ecossistema para compilar código Rust diretamente para SPIR-V (shaders) e interagir com o ecossistema NVIDIA/CUDA (C++ Interop avançado).

---

## Módulo 3: Programação de Sistemas e Baixo Nível (Bare Metal)
*Foco: Interação direta com o SO, bypass do kernel, manipulação de memória crua e eBPF.*

- **Programação de sistemas**: software que conversa diretamente com o sistema operacional, hardware ou infraestrutura de execução.
- **Baixo nível / bare metal**: contexto em que há pouca ou nenhuma abstração entre o seu código e a máquina.
- **Bypass do kernel**, aqui, significa reduzir camadas de abstração padrão quando isso é necessário para desempenho ou controle.
- **eBPF**: tecnologia que permite carregar pequenos programas verificados para rodar dentro do kernel Linux.

*   **Aula 27:** **Rust `#![no_std]` e Bare Metal** — Como remover a dependência da biblioteca padrão (libc/SO) e escrever código que roda diretamente no hardware (útil para sistemas embarcados e desenvolvimento de kernels).
*   **Aula 28:** **Manipulação Direta de Syscalls e mmap** — Bypass das abstrações padrão: como usar `mmap` (Memory-Mapped Files) em Rust para I/O extremamente rápido e interagir diretamente com o Kernel Linux.
*   **Aula 29:** **Programação eBPF no Kernel Linux (Aya)** — Como escrever filtros de pacotes de rede e ferramentas de observabilidade de SO que rodam dentro do Kernel Space com segurança garantida pelo *verifier* do Linux e pelo borrow checker do Rust.

---

## Módulo 4: Projetos Práticos (Masterpieces)
*Foco: Unir todo o conhecimento teórico das aulas 1 a 29 para construir softwares complexos do zero, simulando o trabalho de um Engenheiro de Sistemas Sênior.*

Este módulo existe para fazer a transição entre "entender conceitos" e "construir sistemas completos". Em engenharia, conhecimento só se consolida de verdade quando você integra várias decisões simultaneamente: rede, memória, concorrência, erro, observabilidade e performance.

*   **Aula 30 (Projeto A):** **Construindo um Load Balancer TCP do Zero**
    *   Uso direto da API `epoll` (Linux) / `kqueue` (macOS).
    *   Gerenciamento manual de descritores de arquivo (File Descriptors) não bloqueantes.
    *   Implementação de roteamento Round-Robin de altíssima performance.
*   **Aula 31 (Projeto B):** **Banco de Dados In-Memory Concorrente (Estilo Redis)**
    *   Implementação de um servidor TCP assíncrono.
    *   Estruturas de dados concorrentes com fragmentação de acesso (HashMaps sharded).
    *   Parsing de um protocolo customizado, com caminho de evolução para zero-copy e menos alocações.

---

## Próximos Passos Depois do Roadmap

Agora que as Aulas 20 a 31 já existem, este documento deixa de ser apenas um plano futuro e passa a funcionar como mapa de navegação da fase avançada.

Use-o de quatro formas principais:
1.  [ ] Revisar linearmente a trilha estendida inteira, da Aula 20 até a Aula 31.
2.  [ ] Revisitar um módulo específico: backend distribuído, HPC/GPU, baixo nível ou projetos práticos.
3.  [ ] Reexecutar os projetos das Aulas 30 e 31 com instrumentação, benchmarks e testes próprios.
4.  [ ] Avançar para a extensão opcional em [32_bonus.md](32_bonus.md), que fecha o banco in-memory com zero-copy, `INCR` atômico e expiração TTL.

*Em outras palavras: este roadmap já foi executado. A partir daqui, ele serve como índice da especialização completa e como ponto de partida para aprofundamentos práticos.*
