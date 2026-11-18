---
layout: default
---

# Aula 19 - Plano de Estudos de Especializacao em Go

## Objetivo da Aula
Depois de concluirmos a trilha principal de Go das Aulas 1 a 19, este documento organiza a proxima fase do estudo: sair dos fundamentos da linguagem e entrar em dominios reais de engenharia.

Daqui para a frente, o foco deixa de ser "aprender sintaxe e mecanismos isolados" e passa a ser:

- aplicar Go em sistemas reais;
- aprofundar backend, rede, concorrencia e operacao;
- estudar desempenho com criterio;
- aproximar a linguagem de problemas concretos de infraestrutura e sistemas.

Esta aula nao introduz um recurso novo de Go. Ela organiza a fase seguinte da trilha em modulos de especializacao.

Mapa mental:

- ate a Aula 19, o foco foi construir base solida da linguagem, toolchain, memoria, concorrencia, HTTP, testes e producao;
- a partir daqui, o foco passa a ser dominio de aplicacao;
- cada modulo representa uma familia real de problemas de engenharia que Go resolve muito bem.

---

## Modulo 1: Backend de Alta Performance e Servicos de Rede
*Foco: backend concorrente, servicos HTTP robustos, RPC, banco de dados e comunicacao entre processos.*

- **Backend de alta performance**: software de servidor desenhado para throughput alto, latencia previsivel e uso eficiente de CPU e memoria.
- **Servico de rede**: processo que aceita conexoes, processa requisicoes e se integra com outros componentes via protocolo de rede.
- **RPC**: remote procedure call, isto e, chamada entre processos ou servicos como se fosse uma invocacao local, mas trafegando pela rede.

*   **Aula 20:** **Servidores HTTP de Producao** - `net/http` aprofundado, `ServeMux`, middlewares, timeouts, shutdown gracioso, limites de leitura e escrita, organizacao de handlers e separacao entre camada HTTP e regra de negocio.
*   **Aula 21:** **WebSockets, Streaming e Fan-In/Fan-Out** - conexoes persistentes, broadcast, controle de goroutines por cliente, backpressure e custo de memoria em servicos com milhares de conexoes simultaneas.
*   **Aula 22:** **Banco de Dados, Pools e Transacoes** - `database/sql`, `context`, pool de conexoes, latencia, transacoes, consultas concorrentes, e comparacao entre acesso idiomatico em Go e modelos mais pesados de frameworks.
*   **Aula 23:** **gRPC e Comunicacao entre Servicos** - protobuf, servicos binarios, streaming, timeouts, retries e contratos entre microservicos com foco em backend interno e servicos de infraestrutura.

---

## Modulo 2: Observabilidade, Confiabilidade e Operacao
*Foco: fazer software que nao apenas funciona, mas que tambem pode ser operado, medido, depurado e evoluido.*

- **Observabilidade**: capacidade de entender o comportamento interno do sistema a partir de sinais externos como logs, metricas, traces e profiles.
- **Confiabilidade**: capacidade de continuar operando de forma previsivel mesmo sob falha parcial, carga alta ou dependencia instavel.
- **Backpressure**: mecanismo de limitar ou desacelerar produtores quando consumidores nao acompanham o ritmo.

*   **Aula 24:** **Logs, Metricas, Tracing e pprof em Servicos Reais** - estruturacao de logs, correlacao por request, metricas de latencia e erro, tracing distribuido e uso operacional de `pprof`.
*   **Aula 25:** **Resiliencia: Retries, Circuit Breakers, Timeouts e Quotas** - como evitar cascata de falhas, como limitar dano e como projetar clientes e workers mais robustos.
*   **Aula 26:** **Workers, Filas, Schedulers e Jobs Confiaveis** - processamento assincrono, retries com idempotencia, cancelamento, deadlines, filas internas e integracao com automacao e jobs de infraestrutura.

---

## Modulo 3: Performance, Runtime e Proximidade com o Sistema
*Foco: entender profundamente custo real em CPU, memoria, scheduler, garbage collector e fronteiras com C e syscalls.*

- **Runtime**: camada que executa e coordena goroutines, scheduler, memoria e garbage collector.
- **Hot path**: trecho do codigo percorrido com frequencia ou custo alto, onde pequenas decisoes podem ter impacto relevante.
- **Syscall**: chamada ao kernel para usar servicos do sistema operacional, como rede, arquivo, memoria ou processo.

*   **Aula 27:** **Scheduler, GC e Latencia** - como o runtime decide execucao de goroutines, como o garbage collector afeta throughput e latencia, e como interpretar custo real de alocacao em servicos intensivos.
*   **Aula 28:** **Syscalls, mmap, Arquivos Grandes e I/O Avancado** - uso mais proximo do sistema, memory-mapped files, buffers, custo de copia, limites da biblioteca padrao e quando isso realmente vale a pena.
*   **Aula 29:** **cgo Avancado, ABI e Integracao com Bibliotecas Nativas** - ownership na fronteira Go/C, custo de crossing, callbacks, structs compartilhadas e criterios para integrar com bibliotecas de C/C++ sem destruir simplicidade operacional.

---

## Modulo 4: Projetos Praticos de Engenharia
*Foco: integrar rede, concorrencia, memoria, observabilidade, erro e performance em sistemas completos.*

Este modulo existe para consolidar o conhecimento de verdade. Em engenharia, dominio real aparece quando voce precisa tomar varias decisoes corretas ao mesmo tempo:

- modelo de concorrencia;
- organizacao de packages;
- tratamento de erro;
- custo de memoria;
- operacao em producao;
- medicao e depuracao.

*   **Aula 30 (Projeto A):** **Construindo um Reverse Proxy / Load Balancer em Go**
    *   roteamento de requisicoes;
    *   pools de conexoes;
    *   health checks;
    *   timeout, retry e shedding de carga;
    *   logs, metricas e shutdown gracioso.
*   **Aula 31 (Projeto B):** **Construindo uma Ferramenta de Infraestrutura ou Automacao de Producao**
    *   CLI robusta com configuracao e codigos de saida;
    *   concorrencia para processamento em paralelo;
    *   integracao com HTTP, arquivos e sistema operacional;
    *   perfil de memoria, benchmark e fluxo minimo de qualidade.

---

## Como seguir daqui

Agora que as Aulas 20 a 31 ja existem, voce pode usar este plano de duas formas:

1. seguir linearmente do inicio ao fim da trilha estendida;
2. revisitar um modulo especifico conforme o tipo de sistema que quer consolidar primeiro.

Sugestao pratica por objetivo:

- quer backend e servicos: revise o Modulo 1;
- quer operacao e confiabilidade: revise o Modulo 2;
- quer performance e sistemas: revise o Modulo 3;
- quer consolidar tudo com projeto real: revise o Modulo 4.

---

## Proximos Passos Depois do Plano

Com a trilha estendida completa, os proximos movimentos naturais passam a ser:

1. [ ] Revisar linearmente as Aulas 20 a 31 para consolidar o encadeamento completo.
2. [ ] Reexecutar os projetos praticos das Aulas 30 e 31 com testes e metricas reais.
3. [ ] Escolher um modulo para aprofundar com exercicios extras e experimentos proprios.
4. [ ] Transformar um dos projetos em repositorio executavel com configuracao externa e testes.
5. [ ] Estender a trilha com topicos novos, como cloud, Kubernetes, Kafka, caches distribuidos ou observabilidade avancada.

*Use este documento como mapa de navegacao da especializacao: ele deixou de ser apenas um plano futuro e passou a ser um indice da fase avancada completa da trilha de Go.*
