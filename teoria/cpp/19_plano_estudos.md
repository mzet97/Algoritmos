# Aula 19 - Plano de Estudos de Especializacao em C++

## Objetivo da Aula

Depois de concluirmos a trilha principal de C++ das Aulas 1 a 19, este documento organiza a proxima fase do estudo: sair da base da linguagem e entrar em dominios mais profundos de engenharia nativa, bibliotecas, desempenho, sistemas e software de infraestrutura.

Daqui para a frente, o foco deixa de ser apenas "aprender C++ corretamente" e passa a ser:

- aplicar C++ em sistemas reais;
- aprofundar custo de abstracoes, alocacao e fronteiras binarias;
- estudar modelos modernos de concorrencia e assincronia;
- consolidar engenharia de bibliotecas, runtimes, ferramentas e servicos.

Esta aula nao introduz um recurso novo isolado da linguagem. Ela organiza a fase seguinte da trilha em modulos de especializacao.

Mapa mental:

- ate a Aula 19, o foco foi construir base solida de tipos, ownership, RAII, templates, STL, erro, ABI, debugging, concorrencia, rede e producao;
- a partir daqui, o foco passa a ser dominio de problema e custo de engenharia;
- cada modulo representa uma familia real de sistemas em que C++ continua extremamente forte.

---

## Modulo 1: Assincronia, Rede e Arquiteturas de Execucao
*Foco: event loops, I/O assincrono, modelos de execucao, servidores de baixa latencia e composicao de trabalho concorrente.*

- **Event loop**: laco central que observa fontes de evento, agenda trabalho e despacha continuacoes sem criar um thread por operacao.
- **Reactor/Proactor**: dois modelos classicos de I/O assincrono; no reactor reagimos a readiness, no proactor reagimos a conclusao de operacao.
- **Coroutine**: mecanismo de suspensao e retomada que ajuda a escrever fluxo assincrono com sintaxe mais linear, sem apagar o custo e o contrato real.

*   **Aula 20:** **Event loops, reactor, proactor e o lugar de C++ em I/O assincrono** - como pensar event loop, handlers, fila de tarefas e integracao com APIs nativas sem esconder o mecanismo do SO.
*   **Aula 21:** **`std::future`, `std::promise`, thread pools e composicao de tarefas** - como modelar trabalho concorrente, sincronizacao de resultados e limites de abstracoes baseadas em threads.
*   **Aula 22:** **Coroutines em C++: `co_await`, `co_return`, frame e custo real** - o que coroutines realmente geram, quando ajudam em clareza e quando viram complexidade ou armadilha de lifetime.
*   **Aula 23:** **Servidores modernos em C++: buffers, protocolo, backpressure e arquitetura orientada a eventos** - como juntar parsing incremental, ownership, filas e observabilidade em servidores mais serios.

---

## Modulo 2: Memoria, ABI, Type Erasure e Infraestrutura de Biblioteca
*Foco: custo de alocacao, `pmr`, estabilidade de API/ABI, ocultacao de implementacao, type erasure e desenho de bibliotecas reutilizaveis.*

- **`pmr`**: conjunto da biblioteca padrao para trabalhar com recursos de memoria configuraveis, separando container de politica de alocacao.
- **Type erasure**: tecnica para esconder tipo concreto atras de interface uniforme, como em `std::function`, com custo e flexibilidade negociados explicitamente.
- **PIMPL**: padrao de encapsulamento que reduz acoplamento de header e pode ajudar na estabilidade binaria.

*   **Aula 24:** **`std::pmr`, memory resources e estrategias de alocacao em C++ moderno** - como sair do allocator default quando custo de heap e fragmentacao passam a importar.
*   **Aula 25:** **Type erasure, `std::function`, `any`, `variant` e custo de abstracao** - como escolher entre templates, polimorfismo e apagamento de tipo com criterio tecnico.
*   **Aula 26:** **PIMPL, estabilidade de ABI, versao de biblioteca e fronteiras binarias** - como evoluir bibliotecas sem expor internals demais e sem quebrar consumidores desnecessariamente.
*   **Aula 27:** **Mini framework de biblioteca em C++: contrato, ownership, erro, teste e distribuicao** - como transformar modulos em base reutilizavel com disciplina de engenharia.

---

## Modulo 3: Performance Profunda, Hardware e Diagnostico de Custo
*Foco: cache, layout, alocacao, inlining, vetorizacao, false sharing, medicao e criterio para otimizar abstracoes modernas de C++.*

- **Hot path**: trecho de execucao que concentra custo real e onde pequenas decisoes de layout, copia e despacho podem ter impacto relevante.
- **Inlining e vetorizacao**: transformacoes do compilador que podem ajudar muito, mas dependem de estrutura do codigo, aliasing, layout e forma da abstracao.
- **False sharing**: contencao em cache line compartilhada por threads diferentes, mesmo sem erro logico aparente.

*   **Aula 28:** **Layout de objetos, cache lines, indirecao e custo fisico de abstracoes em C++** - como `vector`, `string`, `shared_ptr`, virtual e type erasure aparecem no comportamento do hardware.
*   **Aula 29:** **Profiling de C++ com `perf`, `callgrind`, counters e relatorios do compilador** - como medir copia, alocacao, branch, misses e gargalos reais em codigo idiomatico de C++.
*   **Aula 30:** **Inlining, vetorizacao, `constexpr`, templates e criterio para otimizar hot paths** - quando o compilador ajuda, quando o design atrapalha e quando simplificar a abstracao rende mais que micro-otimizar assembly.

---

## Modulo 4: Projetos Praticos de Engenharia em C++
*Foco: integrar assincronia, API, ABI, memoria, tooling, teste e operacao em sistemas completos.*

Este modulo existe para consolidar o conhecimento de verdade. Em software nativo com C++, dominio real aparece quando voce precisa tomar varias decisoes corretas ao mesmo tempo:

- API e ownership;
- modelo de execucao;
- politica de erro;
- custo de abstracoes;
- instrumentacao e profiling;
- build, teste e distribuicao;
- contratos binarios e operacionais.

*   **Aula 31 (Projeto A):** **Construindo um Servidor TCP/Event-Driven em C++**
    *   wrappers RAII para sockets e buffers;
    *   event loop com API nativa;
    *   parser incremental e framing;
    *   observabilidade, limites e cleanup robusto;
    *   criterio de alocacao e perfil de hot path.
*   **Aula 32 (Projeto B):** **Construindo uma Biblioteca/CLI de Infraestrutura em C++**
    *   biblioteca com API pequena e ownership claro;
    *   separacao entre header publico e implementacao;
    *   erros coerentes, testes e sanitizers;
    *   build reproduzivel com `cmake` ou `make`;
    *   caminho para interoperabilidade com C e outras linguagens.

---

## Como seguir daqui

Este plano existe para organizar a especializacao futura em C++ de duas formas:

1. seguir linearmente da Aula 20 em diante;
2. escolher um modulo conforme o tipo de problema que voce quer dominar primeiro.

Sugestao pratica por objetivo:

- quer assincronia, rede e arquitetura de execucao: comece pelo Modulo 1;
- quer dominar memoria, bibliotecas e estabilidade binaria: comece pelo Modulo 2;
- quer custo mais proximo do hardware e otimizacao com criterio: comece pelo Modulo 3;
- quer consolidar tudo com projeto real: use o Modulo 4 como fechamento.

---

## Proximos Passos Depois do Plano

Agora que as Aulas 20 a 32 ja existem, este documento deixa de ser apenas um plano futuro e passa a funcionar como mapa de navegacao da fase avancada.

Use-o de cinco formas principais:

1. [ ] Revisar linearmente a trilha estendida inteira, da Aula 20 ate a Aula 32.
2. [ ] Revisitar um modulo especifico: assincronia, biblioteca/ABI, performance ou projetos praticos.
3. [ ] Reexecutar os projetos das Aulas 31 e 32 com testes, sanitizers e benchmark.
4. [ ] Comparar modelos de execucao usando `perf`, `callgrind` e carga concorrente.
5. [ ] Estender a trilha com topicos novos, como executors padronizados, NUMA, low-latency systems, game engine internals, HPC ou metaprogramacao mais profunda.

*Use este documento como mapa de navegacao da especializacao: ele deixou de ser apenas um plano futuro e passou a ser um indice da fase avancada completa da trilha de C++.*
