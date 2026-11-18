---
layout: default
---

# Mapa SDD do livro de C

**Estado:** piloto em revisao; o livro completo ainda nao passou pelo gate editorial. Este mapa liga a [especificacao geral](../ESPECIFICACAO_SDD.md) ao [indice atual](indice.md).

## Entrada e saida

**Entrada:** variaveis, condicionais, lacos e funcoes em Python ou JavaScript. Nenhum conhecimento previo de C, ponteiros, sistemas operacionais ou terminal Unix e pressuposto sem explicacao.

**Saida:** o estudante consegue construir, explicar e testar programas em C com memoria disciplinada, protocolos robustos, concorrencia e desempenho mensuravel; consegue avancar por laboratorios de kernel RISC-V/QEMU e comparar mecanismos x86-64. A especificacao nao promete que um servidor didatico seja pronto para producao.

## Modulos e evidencias

| Modulo | Competencias | Aulas atuais aproveitadas | Lacuna a preencher | Evidencia de aprovacao |
| --- | --- | --- | --- | --- |
| C1: primeira compilacao | FND | 01–03 | ponte Python/JavaScript, ambiente multiplataforma, testes de saida e erro | compilar com warnings e explicar compilacao, linkagem e codigo de saida |
| C2: dados e memoria | MEM, SEC | 04–10 | invariantes de buffer, overflow de tamanho, ownership, tipos de falha e limites da abstracao stack/heap | implementar vetor dinamico, detectar falha com sanitizer e justificar cleanup |
| C3: raciocinio algoritmico | MAT, ALG | 03a, 13, [13a](13a_algoritmos_e_estruturas.md) e [13b](13b_arvores_e_ordenacao.md) entregues (BST, degeneracao medida, mergesort estavel, oraculo qsort, ASan/UBSan) | grafos avancados e balanceamento (AVL/rubro-negra) como aprofundamento opcional sobre a 13b | duas implementacoes medidas com argumentos de corretude |
| C4: maquina e OS | ARQ, OS | 10a, 11–12, 15–16, [16a](16a_processos_e_observacao.md) entregue, 18, [32a](32a_kernel_riscv.md)/kernel entregue (boot, PMP, Sv39, traps, syscall, falta sob demanda, escalonador e 2 harts verificados no QEMU) | sistema de arquivos sobre a base da 32a; comparativo x86-64 entregue na [32b](32b_x86_64_comparativo.md) (Multiboot, modo longo, IDT, KERNEL_X86_OK) | diagnosticar experimento usuario/kernel e concluir etapas de kernel em QEMU |
| C5: concorrencia e rede | CON, NET, SEC | 14, [14a](14a_concorrencia_e_threads.md) entregue, 20–23, [23b](23b_fuzzing_e_limites.md) entregue (fuzz determinístico do parser, 200 mil mutações sob ASan/UBSan), 31 | limites globais medidos (conexões/tempo) sobre a base da 23b e da 31 | servidor com protocolo e testes de falha; nenhuma conexao lenta paralisa as demais |
| C6: distribuicao | DST, SEC | 31 como base de rede; [23a](23a_sistemas_distribuidos.md) entregue (dois nós, log com `fsync`, idempotência por `id`, `SIGKILL`, replay, 17 checks), [23c](23c_relogios_e_particao.md) entregue (Lamport, vetores, concorrência detectada, anti-entropia com ordem total e perda declarada) e [23d](23d_consenso_e_quorum.md) entregue (eleição por maioria, commit por maioria, minoria que não confirma, cura com termo novo; GCC `-Werror`, ASan/UBSan) | RPC sobre a base da 23d; anti-entropia por resumos em vez de logs inteiros | dois nos e teste de atraso/queda/recuperacao com comportamento explicado |
| C7: desempenho/HPC/GPU | HPC, GPU | 17, 24–30, [28a](28a_benchmark_honesto.md), [30a](30a_modelo_gpu.md) e [30b](30b_simd_e_afinidade.md) entregues (SIMD verificado contra double, dispatch AVX2 em runtime, afinidade confirmada, NUMA contado: 1 nó nesta máquina) | NUMA multi-nó (first-touch/interleave) quando houver máquina com 2+ nós; GPU física opcional | benchmark com baseline, correcao, ambiente, variancia e criterio de ganho |
| C8: projetos | OS, CON, NET, DST, HPC, SEC | 31–32, [23a](23a_sistemas_distribuidos.md) (projeto distribuído) e [32a](32a_kernel_riscv.md)/kernel (KERNEL_OK no QEMU) | modelo de ameaças formal dos três projetos (comparativo x86-64 entregue na 32b) | repositorio reproduzivel, testes, modelo de ameacas e relatorio tecnico |

As lacunas nao serao acrescentadas como apendice tardio sem pre-requisito: o indice final as colocara antes da primeira aula dependente, mantendo os caminhos atuais como links estaveis. Os exemplos podem mudar de lugar, mas os titulos e URLs atuais devem continuar encontraveis.

## Piloto editorial

| Aula | O que o leitor deve demonstrar | Risco a testar |
| --- | --- | --- |
| [01](01.md) | Compilar, executar, observar saida e codigo de retorno; explicar o que foi gerado | confusao entre compilar e executar, formato de `printf` |
| [04](04.md) | Usar array, ponteiro e tamanho sem leitura fora do limite | terminador ausente, tamanho incorreto, acesso fora de faixa |
| [07](07.md) | Alocar, redimensionar e liberar preservando o ponteiro original em falha | overflow de tamanho, `NULL`, double free e use-after-free |
| [14](14.md) | Preservar invariante compartilhada com mutex ou atomico adequado | data race, lifetime de argumentos e deadlock |
| [23](23.md) | Consumir stream fragmentado sem assumir que um `recv` corresponde a uma mensagem | entrada truncada, excesso de bytes e protocolo invalido |
| [31](31.md) | Executar e explicar servidor TCP com varios clientes e limites explicitos | cliente lento, desconexao, escrita parcial, SIGPIPE e exaustao |

O piloto aprova o **formato**, nao o livro. Cada aula precisa de pre-requisitos, objetivo observavel, fonte executavel coerente com o texto, comando e saida esperada, implicacao de memoria/seguranca, atividade, pista, solucao separada e checklist. A aprovacao do livro depende tambem de concluir C3, C4, C6 (base entregue na 23a; faltam consenso/quorum e partição resolvida), C7 e C8, ainda ausentes ou parciais.

## Revisão integral — estado (continuação 3)

As 25 aulas numeradas que estavam `pendente` passaram por varredura sistemática: extração de todos os blocos `c` com `main` e compilação com `-std=gnu11 -Wall -Wextra` (25/26 compilam; a 06 é projeto multiarquivo e foi compilada e executada unindo os blocos `calculadora.h/.c` + `main.c`, saída `5`/`20`), mais busca de padrões de risco (`gets`/`strcpy`/`strcat`/`sprintf`/`system`): só a aula 25 usa `strcpy`, com literais que cabem no campo de 32 bytes — anotado em aula que entrada externa pede `snprintf`. O CSV marca essas aulas como `revisao_parcial`/`compilado_gcc`. **O gate do volume continua aberto:** falta, por aula, separar enunciado/pistas/solução em arquivos próprios e executar os exemplos completos extraídos como fonte única (hoje a maioria vive como blocos dentro do Markdown). Tratar esta seção como progresso auditado, não como aprovação. Separação das aulas originais concluída: todas as aulas numeradas de C (01–32, com 01/04/07/14/23/31 nos pares de piloto já existentes) têm enunciado na aula e `exercicios/NN-pistas.md` + `exercicios/NN-solucoes.md` ligados dela, com o código das soluções compilado (`-Werror`) e executado (saídas conferidas: `sizeof` 24/16, troca por ponteiros, cópias byte-idênticas, gdb/ASan/valgrind, biblioteca estática, eco com linha partida e com dois clientes, `open/read/fstat`, API opaca, pipe pai-filho, crescimento 1.5×/2×, arena/pool, vetorização relatada pelo compilador e a loja de configuração com testes). Fontes únicas executadas: `teoria/verificar_fontes_unicas.py c` (na regressão) extrai os programas completos das 32 aulas, monta os projetos multiarquivo da própria aula (header pela guarda + implementação vizinha + main), compila com `-Werror` e executa com stdin fechado e timeout: 27 aulas com todos os programas saindo 0, 15/20 servidores ouvindo em namespace de rede, 11 com a falha intencional (SIGFPE) reproduzida e 14/18/21/23/31 sem programa completo em bloco (cobertas por pontes e projetos). O gate do volume segue aberto pelo restante da rubrica (percorrer o mapa em clone limpo); nada disso marca o volume como `revisado`.
