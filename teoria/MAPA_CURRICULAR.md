---
layout: default
---

# Mapa curricular e rastreabilidade inicial

**Estado:** especificacao dos modulos. As aulas atuais cobrem parte dos resultados; as lacunas assinaladas abaixo ainda nao constituem conteudo publicado.

Leia primeiro a [especificacao SDD](ESPECIFICACAO_SDD.md). Cada livro e autonomo: os fundamentos marcados como comuns serao ensinados de novo com exemplos e riscos proprios de cada linguagem, sem exigir outro volume.

## Progressao de competencias

| Etapa | Competencias | Produto do estudante | Criterio de passagem |
| --- | --- | --- | --- |
| 1. Programar e raciocinar | FND, MAT, ALG | Implementacoes pequenas com invariantes e testes | Explica por que o algoritmo termina, quando esta correto e seu custo |
| 2. Entender a maquina | ARQ, MEM, OS | Experimentos de memoria, processos, I/O e chamadas de sistema | Relaciona resultado observado a arquitetura e limites da linguagem |
| 3. Construir servicos | CON, NET, SEC | Protocolo com cliente e servidor | Sobrevive a fragmentacao, entrada invalida, desconexao e carga limitada |
| 4. Operar em falhas | DST, SEC | Servico replicado ou ferramenta distribuida | Demonstra efeito de atrasos, repeticao, queda e recuperacao |
| 5. Medir e especializar | HPC, GPU, OS | Perfil, experimento paralelo e projeto final | Compara corretude, custo e limites em ambiente reproduzivel |

Nao se avanca pela quantidade de paginas lidas. Cada passagem exige codigo, explicacao e resultado verificavel.

## Cinco livros: cobertura existente e expansao necessaria

| Livro | Base aproveitavel | Modulos novos obrigatorios | Projeto final avaliavel |
| --- | --- | --- | --- |
| [C](c/indice.md) | 01–32: linguagem, memoria, concorrencia POSIX, rede, ABI e desempenho | matematica/algoritmos, arquitetura e memoria virtual, OS completo, falhas distribuidas, multicore/HPC e modelo GPU | kernel RISC-V em QEMU; etapa x86-64; servidor de protocolo defensivo |
| [C++](cpp/indice.md) | 01–32: RAII, STL, templates, ABI, threads, coroutines e projetos nativos | fundamentos matematicos e algoritmicos, arquitetura, kernel e ABI, distribuicao, NUMA/MPI, GPU e seguranca sistematica | kernel didatico RISC-V; comparacao x86-64; biblioteca/servico medido |
| [Rust](rust/indice.md) | 01–18 e 20–32_bonus: ownership, async, FFI, perf, no_std, GPU e projetos de rede | matematica/algoritmos, arquitetura, boot/interrupcoes/paginacao, distribuicao rigorosa, HPC multi-no e auditoria de unsafe/FFI | kernel RISC-V; etapa x86-64; servico replicado com testes de falha |
| [Go](go/indice.md) | 01–31: linguagem, goroutines, runtime, backend, cgo e infraestrutura | algoritmos e matematica, arquitetura/OS de espaco de usuario, protocolos distribuidos, consistencia, HPC e fundamentos GPU | sistema distribuido observavel; ferramenta de diagnostico do SO e benchmark |
| [Python](python/indice.md) | 01–32: linguagem, modelo de objetos, async, backend, automacao e dados | algoritmos e matematica, CPython/memoria/OS, redes, distribuicao, HPC multi-no, GPU e seguranca operacional | plataforma de experimentos distribuidos, observabilidade e computacao numerica |

No nucleo obrigatorio de GPU, o estudante aprende modelo de execucao, transferencias, hierarquia de memoria, granularidade, verificacao e criterio para medir ganho. A execucao em placa especifica e opcional; cada experimento tem uma implementacao de referencia em CPU para verificar corretude.

## Contrato dos modulos a acrescentar

### MAT/ALG — antes de estruturas e otimizacao

- **Explicar:** proposicoes, invariantes, inducao, recursao, grafos, complexidade temporal/espacial e limites de analise assintotica.
- **Implementar:** estruturas lineares, arvores, tabelas hash e algoritmos de busca/ordenacao na linguagem do livro.
- **Depurar e medir:** casos-limite, propriedades, tempo e memoria para diferentes tamanhos de entrada.
- **Aprovar:** duas solucoes corretas do mesmo problema, com prova informal e medicao que nao contradiga a analise.

### ARQ/OS — antes de kernel, mmap e multicore

- **Explicar:** ISA, modo usuario/kernel, interrupcao, syscall, cache, TLB, paginacao, processo, thread e sistema de arquivos.
- **Implementar:** em C/C++/Rust, etapas de kernel emulado; em Go/Python, ferramentas de espaco de usuario que observam e testam esses mecanismos.
- **Depurar e medir:** paginas, falhas, contexto, latencia e consumo de memoria com ferramentas disponiveis na plataforma.
- **Aprovar:** diagnostico reproduzivel de uma falha de memoria/isolamento e explicacao do caminho entre programa, runtime, kernel e hardware.

### CON/NET/DST/SEC — antes de declarar um servico robusto

- **Explicar:** data race, deadlock, ordering, framing, backpressure, falha parcial, idempotencia, replicacao, consistencia, fronteira de confianca e modelo de ameacas.
- **Implementar:** protocolo com limites de bytes/tempo/conexoes e servico que trata repeticao e queda de um componente.
- **Depurar e medir:** corrida, cliente lento, entrada truncada, particionamento ou atraso simulado, exaustao de recursos e recuperacao.
- **Aprovar:** teste do caminho feliz e de falhas documentadas; nenhum cliente unico deve travar o servico inteiro no projeto avancado.

### HPC/GPU — depois de arquitetura e medicao

- **Explicar:** localidade, layout, vetorizacao, afinidade/NUMA, escalabilidade, transferencias host/device e limites de paralelismo.
- **Implementar:** referencia sequencial correta, variante multicore e demonstracao de GPU ou simulacao acessivel.
- **Depurar e medir:** verificar resultados antes de cronometrar; relatar ambiente, tamanho de entrada, variancia e gargalo.
- **Aprovar:** relatorio que justifica quando a versao paralela ou GPU ajuda, quando perde e por que.

## Sequencia de publicacao

O [piloto de C](c/MAPA_SDD.md) fixa o formato em seis aulas representativas. Depois de revisar esse piloto, a mesma rubrica sera aplicada a C inteiro, C++, Rust, Go e Python nessa ordem. Correcoes P0 em qualquer livro ocorrem antes da respectiva vez editorial. Cada volume so muda de **rascunho** para **verificado** quando seu proprio mapa liga todas as competencias acima a aulas, atividades e projetos aprovados.
