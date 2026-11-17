---
layout: default
---

# Mapa SDD do livro de Rust

**Estado:** planejamento curricular e revisão parcial. As aulas existentes são rascunhos; este mapa não as aprova nem declara o livro publicado. Siga a [especificação geral](../ESPECIFICACAO_SDD.md), a [auditoria](../AUDITORIA.md) e o [índice da trilha](indice.md). O [prompt original](prompt.md) define a densidade desejada: explicar o modelo mental, memória, motivos dos mecanismos, exemplos completos e limites, preservando a profundidade das aulas 01 e 02.

## Perfil e resultado

**Entrada:** o leitor sabe usar variáveis, condicionais, laços e funções em Python ou JavaScript. Não se presume conhecimento de C, C++, compilação, terminal Unix, arquitetura de computadores ou Rust. Comparações com C/C++ permanecem úteis, mas não podem ser pré-requisitos ocultos.

**Saída:** o leitor consegue projetar, implementar, depurar e medir software Rust com contratos explícitos de ownership, interfaces `unsafe`/FFI auditáveis, protocolos robustos, concorrência multicore e componentes distribuídos. Consegue concluir um kernel didático RISC-V em QEMU, comparar mecanismos com x86-64 e explicar quando CPU, GPU ou outra arquitetura muda o custo. A aprovação exige artefatos e medições; extensão textual ou checklist marcado pelo próprio autor não bastam.

**Ambiente obrigatório:** toolchain Rust estável (`rustup`, `rustc`, `cargo`), editor, terminal e QEMU para os laboratórios de kernel. Comandos específicos de Unix devem indicar Linux ou WSL; instruções equivalentes para Windows/macOS ou uma VM reproduzível devem acompanhar o núcleo obrigatório. Registre versão exata da toolchain e plataforma em cada lançamento. GPU física, CUDA, eBPF e recursos nightly ficam em laboratórios opcionais.

## Sequência de módulos

Cada módulo tem uma entrega verificável. Onde a coluna de lacunas aponta um laboratório futuro, ele deve ser introduzido **antes** da primeira aula que o exige; os caminhos `01.md` a `32_bonus.md` permanecem estáveis.

| Módulo e pré-requisitos | Competências e aulas existentes | Lacunas e posição curricular | Avaliação necessária |
| --- | --- | --- | --- |
| **R1 — linguagem e execução.** Entrada descrita acima. | FND: [01](01.md)–[02](02.md), [05](05.md)–[06](06.md), [08](08.md). | Ponte explícita Python/JavaScript → compilação, tipos, `Result`, crates e mensagens de erro; instalação reproduzível nas plataformas aceitas. | **R1-A:** criar CLI pequena sem copiar exemplos, executar testes de sucesso/erro e explicar compilação, linkagem, retorno e dependências. |
| **R2 — ownership e representação.** R1. | MEM, SEC: [03](03.md)–[04](04.md), [07](07.md), [10](10.md), [12](12.md), [15](15.md)–[16](16.md). | Desenhos de layout, aliasing e vida útil de referências; vazamento possível em código seguro; limites de `unsafe`, invariantes de coleção, ZST e contrato de alocador em FFI. Ensinar a base segura antes de abrir `unsafe`. | **R2-A:** implementar API segura sobre representação limitada; testar vazio, realocação, liberação e falhas; explicar a invariante de cada bloco `unsafe` e auditar ownership em FFI. |
| **R3 — raciocínio e algoritmos.** R1–R2. | [12a](12a_raciocinio_e_algoritmos.md) entregue: invariantes, indução, busca binária, BFS, hash, testes de propriedade e medição. | Completar ordenação, árvores e custo de alocação/acesso sobre a base da 12a, antes da aula 13. | **R3-A:** provar invariante de um algoritmo, testar casos limite e comparar duas estruturas/implementações com medições reproduzíveis. |
| **R4 — máquina e SO em espaço de usuário.** R2–R3. | [16a](16a_maquina_e_so.md) entregue: processos, syscalls, `/proc`, RSS e localidade; [17](17.md)–[18](18.md), [27](27.md)–[29](29.md) como base. | Completar ISA, ABI, cache, interrupção e escalonamento sobre a base da 16a, antes da aula 17. | **R4-A:** medir efeito de localidade de memória, interpretar página/fault e syscall em experimento reproduzível e explicar separação usuário/kernel. |
| **R5 — concorrência e rede.** R2–R4. | [14a](14a_framing_e_concorrencia.md) entregue: framing, limites, timeouts e teto de conexões; [13](13.md)–[14](14.md), [20](20.md)–[21](21.md), [23](23.md), [30](30.md)–[32 bônus](32_bonus.md) como base. | Completar modelo de memória, ordenação atômica, bloqueio, cancelamento e backpressure sobre a base da 14a, antes dos frameworks. | **R5-A:** servidor e cliente com mensagens fragmentadas/coalescidas, entrada inválida, clientes lentos e quedas; ensaio concorrente preserva invariantes e documenta limites de recursos. |
| **R6 — sistemas distribuídos.** R5. | DST, SEC: [20](20.md)–[23](23.md) introduzem serviços, banco de dados e gRPC; [30](30.md)–[32 bônus](32_bonus.md) integram rede e estado local. | [23b](23b_sistemas_distribuidos.md) entregue após os fundamentos de rede: dois nós, sync_all, idempotência, kill/replay/atraso (10 checks). Faltam relógios/ordem e consenso sobre a base da 23b. HTTP/gRPC não comprovam distribuição resiliente. | **R6-A:** executar serviço em dois nós com atraso, queda, mensagem repetida e recuperação; explicar a garantia de consistência e medir falhas sem duplicar efeitos. |
| **R7 — desempenho, multicore, HPC e GPU.** R3–R5. | [23a](23a_hpc_baseline.md) entregue: baseline, multicore com `scope`, contenção granular, `black_box` e conta da viagem; [17](17.md)–[18](18.md), [24](24.md)–[26](26.md) como base. | Completar vetorização/intrinsics, afinidade/NUMA e multinó sobre a base da 23a; `std::simd` nightly e GPU física seguem opcionais. | **R7-A:** hipótese, baseline correto, benchmark repetível e interpretação de variância; comparar escalar, multicore e GPU quando disponível, incluindo custo de transferência. |
| **R8 — kernel didático.** R2–R5 e R7 básico. | OS, MEM, CON, SEC: [15](15.md), [16](16.md), [27](27.md) e [28](28.md) são preparação, não um kernel concluído. | [28a](28a_kernel_riscv.md)/kernel **entregue e executado** (KERNEL_OK, mesmo teste das trilhas C/C++); comparativo em [28b](28b_x86_64_comparativo.md) (KERNEL_X86_OK). Explicar claramente interfaces arquiteturais, alvo, link script, boot e limites de `unsafe`. | **R8-A:** boot reproduzível, testes de exceção/memória/syscall/scheduler/I/O e multicore; relatório compara mecanismos RISC-V e x86-64, com modelo de ameaças e limitações. |
| **R9 — integração e segurança defensiva.** R1–R8. | SEC, NET, DST, HPC: [15](15.md)–[16](16.md), [20](20.md)–[23](23.md), [30](30.md)–[32 bônus](32_bonus.md). | Revisar entradas hostis, exaustão, privilégios, dependências, segredos/logs, parser e FFI em cada módulo; acrescentar fuzzing, testes de falha, revisão de `unsafe` e política de incidentes controlados. | **R9-A:** projeto final com especificação, teste, medição, modelo de ameaças e correção demonstrada de ao menos uma falha; revisão por pares antes da publicação. |

### Percurso obrigatório do kernel (R8)

1. **Ambiente e boot RISC-V:** alvo estável verificável, QEMU, saída serial, mapa de memória e artefato de boot; registrar versão e comando completo.
2. **Exceções e interrupções:** trap handler mínimo, retorno seguro e teste que exercita exceção esperada.
3. **Memória:** alocador inicial, páginas, tabelas de páginas, isolamento e testes de mapeamento inválido. Documentar invariantes e cada bloco `unsafe`.
4. **Processos e syscalls:** transição de privilégio, validação de ponteiros e buffers, falha de syscall e separação de endereços.
5. **Escalonamento e tempo:** alternância de tarefas, temporizador e teste de justiça/continuidade sob falha.
6. **I/O e armazenamento didático:** console, driver simples e interface de arquivo ou bloco com limites claros; não prometer sistema de arquivos de produção.
7. **Multicore:** inicialização de múltiplos harts, sincronização, atomics/ordenação e teste que detecta perda de atualização ou deadlock.
8. **Comparação x86-64:** repetir os mecanismos observáveis de boot, traps, páginas e multicore em laboratório comparativo ou portar componente delimitado. Não portar código específico de ISA fingindo equivalência de ABI.

Cada etapa precisa de enunciado, pistas graduais, solução comentada em arquivo separado, comando de execução e resultado esperado, teste de falha, rubrica e referência primária. A etapa seguinte só começa com a anterior reproduzível em ambiente limpo.

## Matriz de rastreabilidade e aceite

Os IDs seguem a especificação geral. A coluna “atividade” nomeia a avaliação **a produzir**; a presença de exercícios nas aulas atuais não os substitui. Nenhuma linha está aprovada até que enunciado, pistas, solução, rubrica e evidência de execução estejam publicados.

| ID | Aulas-base | Exercício/experimento exigido | Avaliação |
| --- | --- | --- | --- |
| FND | 01–02, 05–06, 08 | CLI com entradas válidas e inválidas | R1-A |
| MAT | 07, 11 + 12a entregue (invariante e prova) | invariante e prova simples | R3-A |
| ALG | 07, 11 + 12a entregue (busca, BFS, propriedades) | comparar duas estruturas/algoritmos | R3-A |
| ARQ | 17–18, 27–28 + 16a entregue (processos, syscall, /proc, localidade) | experimento cache/página/syscall | R4-A |
| MEM | 03–04, 10, 12, 15–16 | API segura, ZST, realocação e FFI | R2-A |
| CON | 13–14 + 14a entregue + multicore pendente | teste de corrida/invariante e atomics | R5-A, R8-A |
| NET | 20–21, 23, 30–32 bônus + 14a entregue (framing, fragmentação, cliente lento) | framing, fragmentação, cliente lento | R5-A |
| DST | 20–23 + [23b](23b_sistemas_distribuidos.md) entregue | repetição, atraso e recuperação | R6-A |
| OS | 16a entregue (espaço de usuário) + 27–29 + [28a](28a_kernel_riscv.md) kernel executado | QEMU: traps, páginas, syscalls, scheduler | R4-A, R8-A |
| HPC | 17–18, 24 + 23a entregue (baseline, multicore, black_box) | baseline, profiling e speedup | R7-A |
| GPU | 25–26 + 23a entregue (modelo, transferência, veredito) | transferência e comparação de custo | R7-A |
| SEC | 08, 15–16, 20–23, 30–32 bônus | fuzzing, ameaça e falha controlada | R2-A, R5-A, R9-A |

**Gate de módulo:** objetivo observável, pré-requisitos ensinados, fonte completa quando assim declarada, versão e plataforma verificadas, testes de sucesso/falha, memória/segurança e desempenho quando aplicáveis, enunciado → pistas → solução separados, checklist verificável e fonte primária. **Gate do livro:** todos os módulos R1–R9 aprovados, P0/P1 zerados, links válidos, exemplos completos executados e pelo menos um projeto final percorrido do zero em ambiente limpo. Os projetos existentes das aulas 30–32 são pontos de partida, não evidência de que o kernel ou sistemas distribuídos já estão concluídos.

## Achados técnicos prioritários para a revisão

- [03](03.md): distinguir segurança de memória de ausência de vazamentos; ciclos de referências podem manter alocações vivas em Rust seguro. [Fonte primária](https://doc.rust-lang.org/book/ch15-06-reference-cycles.html).
- [15](15.md)–[16](16.md): as correções de ZST, capacidade real e contrato de alocador foram feitas e exemplos selecionados executados, conforme a [auditoria](../AUDITORIA.md). Fazer revisão integral de invariantes, unwind/panic e integração C antes da aprovação; `CString::from_raw` só recebe ponteiro originado em `CString::into_raw`. [Contrato da API](https://doc.rust-lang.org/std/ffi/struct.CString.html#method.from_raw).
- [24](24.md): `std::simd` usa recurso nightly no texto e a aula faz promessas fortes sobre vetorização; rebaixar o laboratório experimental e comprovar assembly/benchmark antes de afirmar instrução emitida. O núcleo usa toolchain estável.
- [27](27.md): o atributo exportado foi atualizado para `#[unsafe(no_mangle)]` nas edições aplicáveis; `no_std` não elimina automaticamente toda dependência de runtime ou define um kernel completo. [Edition Guide](https://doc.rust-lang.org/edition-guide/rust-2024/unsafe-attributes.html).
- [29](29.md): Rust e o verifier eBPF analisam propriedades diferentes; compilação não prova que o programa carregará no kernel de destino. Tratar instalação, privilégio e versão do kernel como pré-requisitos do laboratório opcional. [Verifier do Linux](https://docs.kernel.org/bpf/verifier.html).
- [30](30.md)–[32 bônus](32_bonus.md): executar projetos completos em ambiente limpo, testar framing, desconexão, saturação e persistência/recuperação antes de alegar robustez. A [auditoria por arquivo](../AUDITORIA_POR_ARQUIVO.csv) ainda marca execução como pendente.

**Ordem editorial:** fechar pré-requisitos de R1–R4, endurecer R5, ensinar R6–R7, construir R8 e validar R9. Corrigir achados P0 em qualquer aula imediatamente, mas não declarar aprovação curricular por uma correção isolada.
