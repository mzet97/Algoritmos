---
layout: default
---

# Mapa SDD do livro autônomo de C++

**Estado:** especificação curricular, não aprovação editorial. As aulas [01–32](indice.md) existem, mas ainda não comprovam todas as competências da [especificação geral](../ESPECIFICACAO_SDD.md). Este mapa distingue material existente de aulas, atividades e projetos **planejados**. A [auditoria](../AUDITORIA.md) registra os achados técnicos conhecidos.

## Contrato de entrada e saída

**Entrada:** saber usar variáveis, condicionais, laços e funções em Python ou JavaScript. O livro deve ensinar terminal, compilação, tipos, ponteiros, matemática aplicada e mecanismos do SO dentro do próprio volume; C e Rust não são pré-requisitos. Comparações com essas linguagens enriquecem o texto, mas a explicação de C++ deve ser suficiente sozinha.

**Saída observável:** o estudante consegue justificar correção e custo de algoritmos; escrever C++ moderno com tempo de vida e ownership explícitos; diagnosticar comportamento de memória, CPU e SO; construir um protocolo defensivo e um serviço distribuído tolerante a falhas simuladas; medir paralelismo multicore/HPC; explicar quando uma GPU ajuda; e completar um kernel didático RISC-V em QEMU antes de comparar os mesmos mecanismos em x86-64. Cada alegação requer programa, teste, diagnóstico ou medição reproduzível, não apenas leitura.

**Base de ferramentas:** C++20 estável para o percurso comum, com GCC ou Clang e warnings. A aula [11](11.md) usa `std::expected` de C++23 e biblioteca padrão compatível: é uma exceção explícita, não pré-requisito para o restante do livro. Laboratórios de kernel usam Linux ou WSL com toolchain cruzada e QEMU, após publicar e verificar os comandos exatos; macOS e Windows nativos seguem as aulas portáveis e podem usar a VM indicada. GPU física, MPI e recursos específicos de CPU são extensões opcionais com baseline obrigatório executável em CPU comum. Ferramentas de medição devem registrar versão, plataforma e limitações.

## Sequência e módulos de ponte

As aulas numeradas conservam seus URLs. Os nomes abaixo são **destinos planejados** e ainda não são links nem aulas prontas. Inserir cada ponte no índice antes da primeira aula dependente; não deslocar uma lacuna para um apêndice final.

| Etapa | Material atual aproveitável | Ponte exata a produzir e posição | Pré-requisito para começar | Evidência para passar |
| --- | --- | --- | --- | --- |
| CPP-1: linguagem e compilação | 01–03, [03a](03a.md) entregue | Completar ponte de compilação e ambiente multiplataforma sobre a base da 03a | 01–03 | Programa com laço e entrada de borda, invariante escrito, argumento de terminação e teste que tenta falsificá-lo |
| CPP-2: objetos e ownership | 04–08 | Reforçar nas aulas existentes duração de armazenamento, referências pendentes, exceções durante construção, regra de zero, custo de cópia/movimento e invalidação de iteradores | 03a | Implementar vetor ou recurso RAII, testar cópia, movimento, falha de alocação e ausência de use-after-free com sanitizer |
| CPP-3: algoritmos | 08 e algoritmos da STL | [08a](08a.md) entregue: busca/ordenação à mão contra a STL como oráculo, medidas em três tamanhos; árvores/hash/grafos avançados como aprofundamento | 03a, 04–08 | Duas soluções do mesmo problema com testes de borda, argumento de correção e comparação de tempo e memória em tamanhos crescentes |
| CPP-4: arquitetura | 10–12, 15, 28–30 e [12a](12a.md) entregue | Completar ISA, cache, TLB e paginação sobre a base da 12a | 08a, 10–12 | Experimento de acesso à memória ou syscall que relacione observação, código gerado, runtime e SO, distinguindo hipótese de medida |
| CPP-5: depuração e segurança de memória | 05–06, 11, 13, 17 e 24 | [13a](13a.md) entregue: três falhas reproduzidas sob ASan com teste automatizado, correções por dono/verificação/conta; FFI e modelo de ameaças formais como aprofundamento | 12a–13 | Reproduzir uma falha em entrada controlada, obter diagnóstico, corrigir e documentar ownership e limite de recursos |
| CPP-6: concorrência e SO em espaço de usuário | 14, 16–18, 20–22, 28 e [17a](17a.md) entregue | Completar descritores, arquivos e I/O parcial sobre a base da 17a | 12a–14, 16–17 | Programa com threads e I/O, teste de corrida, correção justificada e relatório que distingue contenção de paralelismo real |
| CPP-7: rede robusta | 18, 20, 23, [31](31.md) como esboço e [23a](23a.md) entregue | Completar fuzzing sistemático, limites globais e TLS/autenticação sobre a base da 23a | 13a, 17a, 18, 20–23 | Cliente lento, linhas fragmentadas e malformadas, escrita parcial e desconexão não impedem outro cliente de receber resposta; limites são medidos |
| CPP-8: sistemas distribuídos | 23, 27, 31 e [23b](23b.md) entregue | Completar RPC, relógios lógicos, consistência além de primário-réplica e TLS/autenticação sobre a base da 23b | 23a–23b, 27 antes da avaliação final | Dois nós preservam contrato documentado sob repetição, atraso e queda; relatório explica o que **não** é garantido |
| CPP-9: HPC multicore e multinó | 15, 21, 24 e 28–30 | [30a](30a.md) entregue: matvec multicore com ganho e perda medidos; multinó/NUMA como aprofundamento simulado | 12a, 14–15, 21, 28–30 | Baseline sequencial correto versus versão multicore, com ambiente, variância, speedup e análise de gargalo; multi-nó pode ser simulado localmente |
| CPP-10: GPU | 30 como base de vetorização | [30b](30b.md) entregue: referência CPU verificada + modelo latência/viagem/kernel com limiar; execução em placa como laboratório opcional | 30a | Referência CPU e modelo/execução GPU opcional produzem mesmo resultado; relatório decide quando a transferência anula o ganho |
| CPP-11: projetos de aplicação | 31–32 | Aplicar o endurecimento de 23a a 31; separar enunciado, pistas e solução comentada para 31–32; integrar falhas de [23b](23b.md) | [23a](23a.md)–[23b](23b.md), 30a–30b | Servidor e biblioteca/CLI reproduzíveis, testes de falha, modelo de ameaças, perfil e documentação de limites |
| CPP-12: kernel RISC-V | [33](33.md)/kernel entregue: mesmo percurso da trilha de C compilado como C++ freestanding, KERNEL_OK no QEMU | `33.md`–`39.md`: boot em QEMU, saída serial, trap/interrupção, páginas e alocador, modo usuário/syscalls, escalonamento, I/O/sistema de arquivos mínimo e multicore; somente após 32 | 12a, 13a, 14, 17a, 24, 30a, 32 | Imagem inicializa em QEMU; cada etapa tem teste de comportamento e falha, mapa de memória e explicação de segurança/isolamento |
| CPP-13: comparação x86-64 | [40](40.md)/kernel-x86 entregue: componente x86-64 executado, KERNEL_X86_OK | `40.md`: comparar boot, ABI, tabelas de páginas, interrupções e SMP; portar um componente pequeno sem prometer portar o kernel inteiro | 33–39 aprovadas | Executar o componente comparativo em emulador x86-64 e explicar diferenças observadas, limites e partes não portadas |

**Política de kernel C++:** o núcleo didático usa um subconjunto freestanding documentado. Cada etapa declara quais recursos da linguagem e da biblioteca padrão estão disponíveis e como inicialização estática, construtores, `new`/`delete`, exceções, RTTI e runtime seriam suportados ou excluídos. Não apresentar `std::thread`, `std::vector`, exceções ou alocação hospedada como disponíveis no kernel antes de implementar seus mecanismos. A primeira execução é RISC-V/QEMU; x86-64 é posterior e comparativa.

As futuras aulas do kernel têm fronteiras definidas: `33.md` faz boot e saída serial; `34.md` trata trap e interrupções; `35.md` implementa gerência de páginas e alocador; `36.md` entra em modo usuário e define syscalls; `37.md` introduz escalonador e troca de contexto; `38.md` integra I/O e sistema de arquivos mínimo; `39.md` testa sincronização e execução multicore. `40.md` compara os mecanismos com x86-64 e porta um componente pequeno. Cada etapa só começa quando a anterior inicializa e passa seus testes em QEMU; toda afirmação sobre portabilidade fica limitada ao componente realmente executado.

## Matriz competência → exercício → avaliação

Cada linha abaixo define uma atividade **a produzir**; citar uma aula existente significa base conceitual, não aprovação da aula. Enunciado, pistas graduais e solução comentada devem viver em arquivos separados, com rubrica e comandos verificáveis. A avaliação prática recebe evidência de correção, modelo conceitual, testes de falha, segurança, medição e limites. O avaliador bloqueia aprovação se correção ou segurança falhar, mesmo que o programa seja rápido.

| ID | Aulas existentes / pontes planejadas | Exercício verificável | Avaliação prática e status |
| --- | --- | --- | --- |
| FND | 01–03, 12; 03a | Compilar programa em dois arquivos, explicar preprocessamento, compilação, linkagem e códigos de saída | Executar em ambiente limpo com warnings; **pendente** |
| MAT | 03; 03a entregue (invariante, término, propriedade) | Declarar invariante e provar terminação/correção de busca ou laço sobre vetor | Defender caso vazio e caso extremo, com testes; **parcial** |
| ALG | 08; 08a | Implementar busca ou estrutura escolhida e compará-la à STL sob entradas adversas | Corrigir, justificar complexidade e medir duas soluções; **lacuna** |
| ARQ | 12, 15, 28–30 e [12a](12a.md) entregue | Medir localidade e relacionar resultado a cache, página e TLB | Relatório reproduzível com hipótese, artefato e limites do contador; **parcial** |
| MEM | 04–06, 13, 17, 24; 13a | Construir RAII para recurso com teste de cópia/movimento e erro | ASan/UBSan ou diagnóstico equivalente, sem dangling nem cleanup duplicado; **parcial** |
| CON | 14, 21, 28 e [17a](17a.md) entregue | Produzir e corrigir corrida com invariante compartilhada | Repetição sob múltiplos threads, diagnóstico de race quando disponível e explicação de ordering; **parcial** |
| NET | 18, 20, 23, 31; 23a | Parser de protocolo com comprimento ou linha, entradas divididas/coalescidas e limite de bytes | Cliente e servidor sob entrada truncada, lenta e malformada; **parcial** |
| DST | 23, 27 e [23b](23b.md) entregue | Completar relógios lógicos e consistência além de primário-réplica sobre a base da 23b | Dois nós com atraso, repetição, queda e recuperação simulados; **parcial** |
| OS | 12, 16–18; 12a, 17a, 33–40 | Medir syscall/processo em espaço de usuário e implementar trap/syscall no kernel | Kernel RISC-V em QEMU, seguido de comparação x86-64; **lacuna** |
| HPC | 15, 21, 28–30; 30a | Particionar computação e testar equivalência com baseline sequencial | Benchmark com escalabilidade, variância e análise de NUMA/contensão; **parcial** |
| GPU | 30; 30b | Modelar kernel de dados e custo de transferências contra referência CPU | Correção e análise de limiar de ganho sem depender de placa; **lacuna** |
| SEC | 05–06, 11, 13, 16–18, 23, 31; 13a, [23a](23a.md)–[23b](23b.md), 33–40 | Traçar fronteiras de confiança, testar entrada hostil, vazamento, exaustão e privilégio | Modelo de ameaças e correções verificadas nos projetos; **parcial** |

## Gates editoriais e riscos conhecidos

1. **Revisão aula por aula:** preservar a densidade de [01](01.md) e [02](02.md), introduzir termos antes do uso e verificar exemplos, comandos, comparações com Python/JavaScript, memória, riscos, atividades e referências primárias. Conteúdo completo precisa de fonte executável publicada; trechos conceituais recebem rótulo.
2. **Toolchain e portabilidade:** registrar versões estáveis testadas por aula. `std::expected` continua marcado C++23; uma alternativa C++20 no núcleo obrigatório deve ser ensinada sem dependência nova. POSIX/Linux, WSL, Windows e macOS devem ser distinguidos; o kernel exige ambiente Linux/WSL/QEMU reproduzível.
3. **Bloqueante atual CPP-01:** [31](31.md) permanece esboço local com sockets bloqueantes, mesmo usando `poll`. Não promovê-lo a servidor escalável nem expô-lo como seguro para rede pública. A aprovação de NET/SEC depende do laboratório 23a e de testes de cliente lento, `EAGAIN`, limites globais e timeouts.
4. **Segurança transversal:** checar overflow, vida de objetos, UAF, exceções atravessando FFI, parsing e exaustão nas aulas correspondentes; em concorrência, checar data races e deadlocks; em distribuídos, autenticação/configuração e falha parcial. Fuzzing é defensivo e local, nunca substitui limites de recursos.
5. **Publicação:** manter o volume em **rascunho** até que todas as linhas da matriz apontem para aula, exercício separado e avaliação executada; não existe aprovação retroativa pela mera presença de 32 arquivos. Projetos de kernel, GPU e multi-nó têm de provar a rota obrigatória em hardware comum antes do lançamento.

Este mapa especifica o trabalho faltante. O [índice](indice.md) continua navegando pelas aulas existentes e deve incorporar as pontes à medida que forem verificadas, sem criar links quebrados para os arquivos ainda planejados.

## Revisão integral — estado (continuação 3)

Varredura das aulas numeradas ainda `pendente`: blocos com `main` extraídos e compilados (C++20/23). Dois defeitos reais foram achados e corrigidos **nas aulas**: o thread pool da aula 21 guardava `packaged_task` (move-only) em `std::function` (exige alvo copiável) — corrigido com `shared_ptr` para a task; e a aula 24 passava `&arena` de novo a `emplace_back` de strings pmr, o que não compila — corrigido usando a propagação do alocador do vetor. A aula 12 é projeto multiarquivo (como a 06 de C) e a 01 tem bloco misto de texto/código: ambas ficam com exemplos `pendente` no CSV até extração como fonte única. O gate do volume segue aberto pelos mesmos motivos da trilha de C: separar atividades por aula e executar fontes extraídas. Separação iniciada (mesmo padrão da trilha de C): aulas 01–22, 24 e 25 com `exercicios/NN-pistas.md` e `exercicios/NN-solucoes.md` ligados da aula e código das soluções compilado com `-Werror` em g++ **e** clang++ e executado; as demais numeradas seguem pendentes.
