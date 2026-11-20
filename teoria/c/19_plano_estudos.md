---
layout: default
---

# Aula 19 - Plano de Estudos de Especializacao em C

## Nesta aula

Depois de concluirmos a trilha principal de C das Aulas 1 a 19, este documento organiza a proxima fase do estudo: sair da base da linguagem e entrar em dominios mais profundos de engenharia nativa, sistemas, rede e software de infraestrutura.

Daqui para a frente, o foco deixa de ser apenas "aprender C corretamente" e passa a ser:

- aplicar C em sistemas reais;
- aprofundar fronteiras com kernel, rede e hardware;
- estudar desempenho com criterio mais agressivo;
- consolidar engenharia de bibliotecas, runtimes, ferramentas e servicos.

Esta aula nao introduz um recurso novo da linguagem. Ela organiza a fase seguinte da trilha em modulos de especializacao.

Mapa mental:

- ate a Aula 19, o foco foi construir base solida de sintaxe, memoria, ponteiros, UB, debugging, ABI, concorrencia, sockets e producao;
- a partir daqui, o foco passa a ser dominio de problema;
- cada modulo representa uma familia real de sistemas em que C continua extremamente forte.

---

## Modulo 1: Sistemas de Rede, IPC e Servidores de Alto Desempenho
*Foco: software de rede, servidores mais proximos do SO, eventos, multiplexacao e throughput alto.*

- **Servidor de alto desempenho**: processo desenhado para lidar com muitas conexoes, pouco overhead por request e uso eficiente de CPU, memoria e syscalls.
- **IPC**: comunicacao entre processos na mesma maquina por pipes, sockets locais, memoria compartilhada e mecanismos do sistema operacional.
- **Event loop**: modelo em que um ou poucos loops monitoram varios descritores e despacham trabalho conforme eventos chegam.

*   **Aula 20:** **`select`, `poll`, `epoll` e multiplexacao de I/O** - como sair do modelo bloqueante simples e observar varios descritores ao mesmo tempo com criterio de escalabilidade.
*   **Aula 21:** **Servidores orientados a eventos vs thread-per-connection** - comparacao entre modelos classicos, custo de contexto, heap, stacks e latencia sob carga.
*   **Aula 22:** **IPC, pipes, Unix domain sockets e memoria compartilhada** - mecanismos locais de comunicacao entre processos, custo de copia e desenho de arquitetura em sistemas nativos.
*   **Aula 23:** **Protocolo de aplicacao, framing e parsing robusto** - como definir mensagens, lidar com leituras parciais, limites, erros de protocolo e backpressure.

---

## Modulo 2: Memoria, Alocadores e Estruturas de Runtime
*Foco: heap real, allocators, arenas, pools, layout de objetos e custo de gerenciamento manual em escala maior.*

- **Allocator**: componente responsavel por reservar e liberar memoria dinamica para o programa.
- **Arena allocator**: estrategia em que varios objetos sao alocados em bloco e liberados de uma vez.
- **Object pool**: reaproveitamento de objetos para reduzir custo de alocacao frequente e fragmentacao.

*   **Aula 24:** **Heap real, fragmentacao e comportamento do allocator** - como `malloc` e `free` se comportam sob carga, reuse, fragmentacao externa e interna.
*   **Aula 25:** **Arenas, pools e estrategias customizadas de alocacao** - quando o allocator geral nao basta e como desenhar ownership por lote com mais previsibilidade.
*   **Aula 26:** **Strings, buffers e builders eficientes em C** - montagem incremental de dados, crescimento amortizado e reducao de copia desnecessaria.
*   **Aula 27:** **Mini runtime de containers e APIs reutilizaveis** - como desenhar vetores, filas, mapas simples e contratos genericos via `void *`, callbacks e ownership explicito.

---

## Modulo 3: Performance Profunda, CPU e Proximidade com Hardware
*Foco: custo em nivel de cache line, branch, SIMD, NUMA e medicao de hardware real.*

- **SIMD**: instrucoes que operam em varios dados por instrucao, explorando paralelismo de dados no processador.
- **NUMA**: arquitetura em que acesso a memoria depende da proximidade entre CPU e banco de memoria.
- **Hot path**: trecho de execucao percorrido com frequencia ou custo alto, onde pequenas decisoes podem ter impacto relevante.

*   **Aula 28:** **Cache lines, prefetch, branch prediction e false sharing em profundidade** - como o hardware responde a padroes de acesso e concorrencia.
*   **Aula 29:** **Profiling de CPU e memoria com `perf`, `callgrind` e counters** - como sair da intuicao e entrar em diagnostico real de gargalo.
*   **Aula 30:** **SIMD, vetorizacao e criterio para otimizar loops criticos** - quando vale usar intrinsics, quando o compilador ajuda e quando isso vira complexidade desnecessaria.

---

## Modulo 4: Projetos Praticos de Engenharia
*Foco: integrar rede, memoria, ABI, performance, tooling e operacao em sistemas completos.*

Este modulo existe para consolidar o conhecimento de verdade. Em software nativo, dominio real aparece quando voce precisa tomar varias decisoes corretas ao mesmo tempo:

- API e ownership;
- modelo de concorrencia ou eventos;
- custo de heap;
- controle de erro;
- instrumentacao;
- build e distribuicao;
- limites operacionais e de seguranca.

*   **Aula 31 (Projeto A):** **Construindo um Servidor TCP/Event-Driven em C**
    *   loop de eventos;
    *   `select`, `poll` ou `epoll`;
    *   buffers por conexao;
    *   protocolo simples com framing;
    *   logs, limites e cleanup robusto.
*   **Aula 32 (Projeto B):** **Construindo uma Biblioteca/CLI de Infraestrutura em C**
    *   biblioteca com API opaca;
    *   ownership explicito e codigos de erro;
    *   CLI consumindo a biblioteca;
    *   testes com sanitizer;
    *   build reproduzivel e contrato operacional minimo.

---

## Como seguir daqui

Este plano existe para organizar a especializacao futura em C de duas formas:

1. seguir linearmente da Aula 20 em diante;
2. escolher um modulo conforme o tipo de problema que voce quer dominar primeiro.

Sugestao pratica por objetivo:

- quer software de rede e IPC: comece pelo Modulo 1;
- quer dominar heap e ownership em escala maior: comece pelo Modulo 2;
- quer performance mais proxima do hardware: comece pelo Modulo 3;
- quer consolidar tudo com projeto real: use o Modulo 4 como fechamento.

---

## Proximos Passos Depois do Plano

Agora que as Aulas 20 a 32 ja existem, este documento deixa de ser apenas um plano futuro e passa a funcionar como mapa de navegacao da fase avancada.

Use-o de cinco formas principais:

1. [ ] Revisar linearmente a trilha estendida inteira, da Aula 20 ate a Aula 32.
2. [ ] Revisitar um modulo especifico: rede, memoria, performance ou projetos praticos.
3. [ ] Reexecutar os projetos das Aulas 31 e 32 com `Makefile`, testes, carga e sanitizers.
4. [ ] Comparar decisoes de desenho usando benchmark, profiler e medicao de latencia.
5. [ ] Estender a trilha com topicos novos, como eBPF, drivers, kernel modules, userspace networking ou sistemas embarcados.

*Use este documento como mapa de navegacao da especializacao: ele deixou de ser apenas um plano futuro e passou a ser um indice da fase avancada completa da trilha de C.*
