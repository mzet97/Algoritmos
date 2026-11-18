---
layout: default
---

# Mapa SDD do livro de Go

**Estado:** especificação curricular; nenhum módulo ou livro foi aprovado para publicação. Este mapa aplica a [especificação geral](../ESPECIFICACAO_SDD.md) ao [índice das aulas existentes](indice.md). A marcação de uma aula como base aproveitável **não** atesta sua precisão técnica, segurança ou execução integral.

## Entrada, saída e percurso

**Entrada:** o leitor conhece variáveis, condições, laços e funções em Python ou JavaScript. O livro explica terminal, compilação, módulos, testes e os mecanismos de sistemas antes de exigir seu uso. Não pressupõe C, Rust, Linux ou experiência com serviços de produção.

**Saída observável:** o leitor consegue justificar algoritmos, medir programas Go, investigar a interação entre runtime e sistema operacional, construir serviços concorrentes e distribuídos que suportem falhas parciais e entrada hostil, e explicar limites de CPU, memória, rede e GPU. Go ensina os mecanismos do SO por experimentos de **espaço de usuário** e pela observação do runtime; a avaliação não exige nem promete implementar um kernel em Go.

O percurso obrigatório é G1 → G2 → G3 → G4 → G5 → G6 → G7 → G8. Aulas existentes 01–31 são material de partida; tópicos novos serão inseridos **antes** da primeira dependência, com links estáveis para os arquivos atuais. Cada módulo termina em código, explicação e medição ou teste reproduzível. Laboratórios que exigem Linux, cgo, cluster ou GPU física são opcionais e terão alternativa obrigatória com biblioteca padrão e CPU.

## Módulos, pré-requisitos e aprovação

| Módulo | Competências | Pré-requisitos e base existente | Lacuna específica a produzir | Evidência de aprovação |
| --- | --- | --- | --- | --- |
| **G1. Programar em Go** | FND | Entrada do livro; [01–03](01.md), [04–08](04.md) | Ponte explícita a Python/JavaScript para tipos, cópia, erros, módulos e execução; ambiente limpo em Windows, macOS e Linux; distinguir código fonte, compilação, linkedição e processo | Programa com módulo, entrada validada, erro e testes; estudante explica tipos, fluxo, código de saída e o que `go run` faz |
| **G2. Raciocinar sobre programas** | MAT, ALG | G1 + [08a](08a_raciocinio_e_algoritmos.md) entregue; [04](04.md), [05](05.md), [12](12.md), [14](14.md) fornecem estruturas e testes iniciais | Lógica, conjuntos, relações, indução, invariantes, recursão e prova de terminação; notação assintótica antes de otimização; listas, árvores, grafos, tabela hash, busca e ordenação; testes por propriedades e casos extremos | [08c](08c_duas_solucoes.md) entregue: BST + ordenação, duas soluções com invariante e medição |
| **G3. Máquina, memória e SO** | ARQ, MEM, OS | G2 + [08b](08b_maquina_memoria_e_so.md) entregue; [09](09.md), [13](13.md), [17](17.md), [18](18.md), [27](27.md), [28](28.md), [29](29.md) | ISA/ABI, cache/TLB, paginação, memória virtual, privilégios, interrupção/trap, syscall, processo/thread, escalonamento e sistema de arquivos **antes** de `mmap`/cgo avançado; distinguir memória gerenciada, `unsafe`, memória C e mapeamento; laboratório portátil de processo/I/O e extensão Linux opcional | [08d](08d_ferramenta_so.md) entregue: ferramenta observa goroutines, heap, threads e page faults (Linux), com limitações declaradas |
| **G4. Concorrência multicore** | CON, MEM, SEC | G3 + [11a](11a_modelo_de_memoria.md) entregue; [10](10.md), [11](11.md), [15](15.md), [26](26.md), [27](27.md) | Modelo de memória e relação *happens-before*, ownership de canais, cancelamento e shutdown, corrida de dados vs corrida lógica, deadlock, contenção, fila limitada, escalabilidade com múltiplas CPUs; custo e restrições do race detector | Worker pool limitado com invariantes explícitas; teste força interleavings, cancelamento e falha; `go test -race` quando disponível, comparação entre 1 e múltiplos `GOMAXPROCS`, explicação do ganho e do limite |
| **G5. Redes e protocolos defensivos** | NET, SEC | G4; [16](16.md), [20](20.md), [21](21.md), [23](23.md), [25](25.md), [30](30.md) | TCP é fluxo de bytes: framing, leitura/escrita parcial, tamanho máximo, deadlines e backpressure; limites de corpo/conexões, parsing estrito, TLS e identidade, confiança em cabeçalhos/proxy, SSRF e abuso de recursos; HTTP/gRPC não substituem o estudo do protocolo subjacente | Cliente e servidor de protocolo pequeno passam testes com bytes fragmentados/coalescidos, mensagem truncada/malformada, cliente lento, timeout e carga limitada; modelo de ameaças e limites documentados |
| **G6. Sistemas distribuídos sob falha** | DST, NET, SEC | G5 + [23a](23a_protocolos_distribuidos.md) entregue (G5+G6 numa ponte); [22](22.md), [23](23.md), [24](24.md), [25](25.md), [26](26.md), [30](30.md), [31](31.md) introduzem persistência, comunicação e operação | Relógios e ordem causal, falha parcial, entrega/repetição, idempotência persistida, transações, replicação, quorum e consistência, particionamento, recuperação, testes de falha determinísticos; distinguir demonstração didática de consenso correto | Serviço da 23a + [23b](23b_relogios_e_consistencia.md) entregue: relógios, partição bilateral e política de consistência com perda declarada |
| **G7. Desempenho, HPC e GPU** | HPC, GPU, ARQ | G3–G6 + [28a](28a_hpc.md) entregue (baseline, escala, contenção, conta da viagem); [14](14.md), [17](17.md), [24](24.md), [27](27.md)–[29](29.md) como base | [28b](28b_vetorizacao_e_numa.md) entregue: empate medido com a soma simples, NUMA contado; multinó simulado declarado | Algoritmo numérico com referência sequencial, variante multicore e modelo de custo GPU; testes comparam resultados antes de cronometrar; relatório mostra onde paralelizar ajuda ou piora e por quê |
| **G8. Projetos de conclusão** | FND–SEC | G1–G7; [30](30.md) e [31](31.md) são protótipos aproveitáveis | [31a](31a_projeto_integrador.md) entregue: integração das três peças sob rubrica, com regress como ambiente limpo | Repositório reproduzível com `go test`, testes de integração/falha, medição, documentação de plataforma, modelo de ameaças e defesa oral/escrita das escolhas; aprovação pela rubrica abaixo |

G2 deve anteceder benchmarks e otimizações. G3 deve anteceder explicações que dependem de `mmap`, ABI, scheduler e GC. G5 deve anteceder G6; retries só são ensinados junto de idempotência e limites. As aulas atuais podem ser revisitadas durante o percurso, mas não são pré-requisito oculto de outro livro.

## Matriz de rastreabilidade das competências

Nesta matriz, **existente** significa que há uma aula com material relevante, e **a produzir** nomeia a atividade e a avaliação que faltam. Exercícios e avaliações propostos ainda não estão publicados; portanto a matriz não declara competências concluídas.

| ID | Aula existente / ampliação necessária | Exercício a publicar | Avaliação prática |
| --- | --- | --- | --- |
| FND | [01–08](01.md); ponte Python/JavaScript e ambiente limpo | Interpretar saída, erro, tipo e código de retorno de programa próprio | G1: módulo executável explicado |
| MAT | [08a](08a_raciocinio_e_algoritmos.md) entregue + ampliação G2 antes de [14](14.md) | Provar invariante e terminação de busca | G2: argumento de correção revisado |
| ALG | [04](04.md), [05](05.md), [12](12.md) + [08a](08a_raciocinio_e_algoritmos.md) entregue | Implementar e comparar duas estratégias | G2: correção e complexidade medidas |
| ARQ | [08b](08b_maquina_memoria_e_so.md) entregue + [09](09.md), [17](17.md), [27](27.md), [28](28.md) | Prever e medir efeito de localidade | G3/G7: relatório com hipótese e observação |
| MEM | [04](04.md), [08b](08b_maquina_memoria_e_so.md) entregue, [09](09.md), [18](18.md), [29](29.md) | Localizar retenção de memória e erro de lifetime em fronteira cgo conceitual | G3: diagnóstico e correção segura |
| CON | [10](10.md), [11](11.md) + [11a](11a_modelo_de_memoria.md) entregue, [15](15.md), [26](26.md) | Corrigir corrida e vazamento de goroutine | G4: worker pool sob cancelamento e carga |
| NET | [16](16.md), [20](20.md), [21](21.md), [23](23.md); framing e limites G5 | Testar leitura fragmentada e cliente lento | G5: protocolo cliente/servidor robusto |
| DST | [22](22.md), [23](23.md) + [23a](23a_protocolos_distribuidos.md) entregue, [25](25.md), [26](26.md) | Deduplicar operação após retry e reinício | G6: dois nós sob falha injetada |
| OS | [08b](08b_maquina_memoria_e_so.md) entregue + [13](13.md), [18](18.md), [27](27.md), [28](28.md) | Comparar I/O, processo e métricas de memória entre plataformas | G3/G8: ferramenta de espaço de usuário explicada |
| HPC | [14](14.md), [17](17.md), [27](27.md) + [28a](28a_hpc.md) entregue | Medir escalabilidade e falso compartilhamento | G7: relatório reprodutível |
| GPU | [28a](28a_hpc.md) entregue (modelo, transferência, veredito) | Estimar transferência e ponto de equilíbrio para kernel paralelo | G7: comparação teórica/CPU; hardware opcional |
| SEC | [07](07.md), [15](15.md), [16](16.md), [25](25.md), [30](30.md); revisão transversal G3–G8 | Testar parsing hostil, exaustão e vazamento de dados em logs | G5/G8: modelo de ameaças, teste de abuso e correção |

## Contrato dos projetos e gate editorial

O projeto principal de G8 será um **serviço replicado de tarefas** em dois processos/nós, com API delimitada, registro persistente de operações, chave de idempotência, recuperação após reinício, limites de fila e concorrência, logs estruturados sem dados sensíveis e métricas úteis. O cenário de avaliação injeta atraso, duplicação, desconexão e queda de um nó. O relatório deve declarar a garantia real oferecida e o comportamento em partição; não atribuir linearizabilidade ou consenso ao projeto sem prova e testes correspondentes. [30](30.md) fornece experiência de proxy e [31](31.md) de CLI operacional, mas nenhum dos dois, isoladamente, satisfaz esse projeto final.

O segundo entregável é uma **ferramenta de diagnóstico do SO em Go** que observa processo, memória, CPU e I/O por APIs portáteis; extensões específicas de Linux, como `/proc`, são opcionais e rotuladas. O terceiro é um **experimento numérico** com baseline sequencial e versão multicore, validação de resultados e análise do custo esperado de GPU; execução em GPU física é opcional. Nenhum projeto depende de serviço pago, hardware especial ou dependência externa no caminho obrigatório.

Rubrica comum (100 pontos): correção e invariantes **25**; explicação conceitual **20**; testes de falha e recuperação **20**; segurança defensiva **15**; medição reproduzível **10**; comunicação de limites, ambiente e decisões **10**. Aprovação requer ao menos **70/100**, nenhum teste crítico de segurança/correção reprovado e evidência em todas as seis dimensões. Uma solução apenas para o caminho feliz não aprova.

Antes de publicar o volume: concluir revisão técnica e didática de **todas** as aulas, zerar P0/P1, separar enunciados/pistas/soluções, executar exemplos completos com versão estável registrada, verificar links e percorrer os três entregáveis em ambiente limpo. Registrar `go test ./...`, `go vet ./...`, benchmarks relevantes e `go test -race ./...` onde o race detector for suportado; declarar explicitamente qualquer plataforma ou laboratório não testado. A [auditoria por arquivo](../AUDITORIA_POR_ARQUIVO.csv) permanece a fonte de status granular: hoje as aulas [16](16.md) e [27–31](27.md) têm revisão parcial, e as demais seguem pendentes. Compilar um exemplo não equivale a aprovar a aula ou o livro.

## Revisão integral — estado (continuação 3)

Varredura das aulas numeradas `pendente`: blocos com `package main` analisados com `gofmt -e` (27/29 com parse válido; 2 são blocos mistos de texto/código da extração, não defeitos de aula). CSV em `revisao_parcial`. Gate aberto: separar enunciado/pistas/solução por aula e executar as fontes extraídas como programas únicos. Separação iniciada nos moldes das trilhas anteriores: aulas 01–31 com `exercicios/NN-pistas.md` e `exercicios/NN-solucoes.md` ligados da aula, programas executados com `go vet` + `go run -race` e o projeto multi-package da aula 08 montado e testado (incluindo o erro de símbolo não exportado).
