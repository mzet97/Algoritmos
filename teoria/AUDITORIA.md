---
layout: default
---

# Auditoria editorial e técnica

**Estado:** inventário estrutural concluído; revisão técnica por aula em andamento. A presença de um arquivo não indica que seu conteúdo foi aprovado pelo contrato SDD.

O [registro por arquivo](AUDITORIA_POR_ARQUIVO.csv) lista os 173 documentos originais e a nova aula 3A de C. `estrutura=ok` significa somente que a varredura automática da aula encontrou blocos de código balanceados e cabeçalho de exercícios. Revisões parciais e execução de exemplos aparecem em colunas separadas; nenhuma delas, isoladamente, aprova o livro. Nenhum arquivo está marcado como publicado.

## Inventário inicial

| Livro | Documentos Markdown | Aulas numeradas | Situação observada |
| --- | ---: | ---: | --- |
| C | 35 | 32 | Linguagem e sistemas Unix até servidor TCP e biblioteca/CLI |
| C++ | 35 | 32 | RAII, STL, ABI, concorrência, desempenho e projetos nativos |
| Go | 34 | 31 | Linguagem, concorrência, serviços e runtime |
| Python | 35 | 32 | Linguagem, automação, backend e dados |
| Rust | 34 | 31, incluindo `32_bonus.md` | Ownership, sistemas, GPU e projetos de rede |
| **Total** | **173** | **158** | Cinco trilhas sem um percurso curricular validado |

Cada livro possui `prompt.md`, `indice.md` e `19_plano_estudos.md`. O Rust usa `19_plano_estudos.md` como transição após a aula 18, sem `19.md`; isso deve estar explícito no índice e não ser tratado automaticamente como arquivo ausente.

Uma varredura estrutural dos 158 arquivos numerados encontrou delimitadores de bloco de codigo balanceados e cabecalhos de exercicios em todos eles. Faltam checklists explicitos em `rust/31.md` e `rust/32_bonus.md`; a revisão do conteúdo desses blocos ainda e necessária.

## Achados transversais

| ID | Gravidade | Evidência | Ação de fechamento |
| --- | --- | --- | --- |
| CUR-01 | P1 | Os índices organizam linguagens, mas não mapeiam competência → aula → exercício → avaliação | Publicar matriz em cada livro e inserir aulas de ponte antes de conceitos dependentes |
| CUR-02 | P1 | As trilhas avançam para runtime, kernel, HPC/GPU e serviços sem percurso comum de matemática discreta, arquitetura, OS e distribuídos dentro de cada livro | Inserir módulos próprios por livro, respeitando o contrato de autonomia |
| PED-01 | P1 | Os prompts pedem exemplos completos e exercícios, mas soluções, pistas e rubricas não estão separadas das aulas | Criar atividades em três camadas e critérios de aprovação por módulo |
| PED-02 | P2 | Os livros partem do zero na linguagem, mas quase não usam JavaScript como ponte para o público descrito | Adicionar comparações curtas e precisas na entrada de cada livro |
| EXE-01 | P1 | `teoria/` contém somente Markdown; comandos de vários projetos pressupõem arquivos de código não publicados | Publicar ou extrair exemplos completos e executá-los em ambiente limpo |
| SEC-01 | P1 | Fuzzing, modelo de ameaças, limites de recursos e falhas parciais não formam uma sequência explícita em nenhum índice | Integrar segurança defensiva aos módulos e aos projetos |
| PUB-01 | P2 | Não há entrada geral para `teoria/` nem publicação de site no estado inicial | Criar navegação Markdown e site somente para volumes aprovados |

## Achados específicos confirmados ou em revisão

| ID | Gravidade | Local | Problema e fechamento esperado |
| --- | --- | --- | --- |
| RUS-01 | P0 | [Rust 16](rust/16.md) | Uma tabela sugere liberar memória de `malloc` por `CString::from_raw`. A [API oficial](https://doc.rust-lang.org/std/ffi/struct.CString.html#method.from_raw) exige ponteiro criado por `CString::into_raw`; corrigir o contrato de alocador. |
| RUS-02 | P0 | [Rust 15](rust/15.md) | O `MeuVec<T>` genérico usa `alloc`/`realloc` também para tipo de tamanho zero; revisar o contrato à luz de [`Layout`](https://doc.rust-lang.org/std/alloc/struct.Layout.html). |
| RUS-03 | P1 | [Rust 03](rust/03.md), [Rust 27](rust/27.md) | Distinguir segurança de memória de ausência de vazamentos e atualizar exemplos `no_mangle` para a edição alvo. Fontes: [Rust Book](https://doc.rust-lang.org/book/ch15-06-reference-cycles.html) e [Edition Guide](https://doc.rust-lang.org/edition-guide/rust-2024/unsafe-attributes.html). |
| C-01 | P1 | [C 14](c/14.md) | A afirmação de que threads não compartilham stack precisa distinguir *stack própria por thread* de acesso entre threads ao mesmo espaço de endereçamento, respeitando tempo de vida e sincronização. |
| C-02 | P1 | [C 31](c/31.md) | O servidor anunciado como orientado a eventos usa `send_all` potencialmente bloqueante; validar uma saída com buffer/não bloqueio ou nomear o exemplo como esqueleto com limitação explícita. |
| C-03 | P1 | [C 26](c/26.md) | A concatenação ingênua era descrita somente como lenta, apesar de não verificar overflow de tamanho nem falha de alocação; agora está rotulada como exemplo conceitual inseguro, restando validar o builder seguro da aula. |
| CPP-01 | P1 | [C++ 31](cpp/31.md) | O servidor com `poll` também usa operações bloqueantes e API Unix; sinalizar limites e completar a política de I/O antes de tratá-lo como servidor event-driven robusto. |
| CPP-02 | P1 | [C++ 11](cpp/11.md), [prompt de C++](cpp/prompt.md) | `std::expected` é C++23, enquanto o padrão frequentemente indicado é C++20; explicitar versão e alternativa. |
| GO-01 | P1 | [Go 27](go/27.md) | A recomendação de `GOMAXPROCS(NumCPU())` requer revisão diante de quotas de container e comportamento atual do runtime. |
| GO-02 | P1 | [Go 29](go/29.md) | O exemplo que toma `&valores[0]` precisa tratar slice vazio e explicar tempo de vida nas fronteiras cgo. |
| PY-01 | P1 | [Python 30](python/30.md) | Objetivo menciona persistência, mas o exemplo guarda tarefas apenas em memória; alinhar promessa, limites e testes. |
| PY-02 | P1 | [Python 31](python/31.md), [Python 32](python/32.md) | Comandos e texto anunciam projetos completos, porém parte do código ignora entrada ou pressupõe arquivo não publicado; corrigir ou rotular como trecho conceitual. |

Os achados são a fila inicial, não um laudo final de correção para todas as aulas. Ao resolver cada item, registrar comando executado, resultado, plataforma e revisão do modelo conceitual; remover o item da lista de bloqueantes apenas com evidência.

## Verificação independente em 2026-11-17 (todos os P0/P1)

Releitura integral dos 12 achados contra o texto atual, com execução onde aplicável (WSL Ubuntu, GCC 13, g++ 13, rustc 1.95/1.100, Go 1.22, Python 3.12):

| ID | Veredito | Evidência |
| --- | --- | --- |
| RUS-01 (P0) | FECHADO | [Rust 16](rust/16.md) (linhas ~422-430): tabela de contratos de alocador + "nunca `CString::from_raw`" em ponteiro de `malloc` + link para a API oficial. |
| RUS-02 (P0) | FECHADO | [Rust 15](rust/15.md) (linhas ~885-914): ZST rejeitados por `assert!` com referência a `Layout`; exemplo executado com `rustc 1.95`, edição 2024. |
| RUS-03 (P1) | FECHADO | [Rust 03](rust/03.md) (linhas ~828-836): "ciclos de `Rc` e `mem::forget` ainda podem vazar" + link ao livro oficial; [Rust 27](rust/27.md) usa `#[unsafe(no_mangle)]` da edição 2024. |
| C-01 (P1) | FECHADO | [C 14](c/14.md) (linhas ~239-240 + exercício 5): stack própria por thread *e* espaço compartilhado com tempo de vida; aula [14a](c/14a_concorrencia_e_threads.md) reforça com `join` antes do retorno. |
| C-02 (P1) | FECHADO | Servidor [C 31](c/31.md) reescrito: `O_NONBLOCK` + fila + `POLLOUT`; código ([aula31.c](c/exemplos/aula31.c)) verificado por leitura integral — um único `send` parcial com `EAGAIN`, sem `send_all`. Histórico da versão bloqueante documentado na aula; `test_aula31.py` passa 7/7. |
| C-03 (P1) | FECHADO COM EVIDÊNCIA NOVA | Trecho inseguro rotulado (aula 26, linha ~113-115); builder `StrBuilder` extraído (linhas 285-376), compilado com `-std=c11 -Wall -Wextra -Wpedantic -Werror`, executado (`len=54 cap=64` corretos) e limpo sob ASan/UBSan. Fecha o item "builder ainda exige verificação". CSV da aula atualizado para `executado_gcc`. |
| CPP-01 (P1) | FECHADO | [C++ 31](cpp/31.md) rotulado "esboço executável para testes locais" em loopback, com seção de limites e laboratório de endurecimento; exemplo verificado ao vivo (eco responde, contrato `ECHO x -> x`). |
| CPP-02 (P1) | FECHADO | [C++ 11](cpp/11.md) declara `std::expected` como C++23 com fallback documentado; exemplo completo compilado e executado com `-std=c++23`. |
| GO-01 (P1) | FECHADO | [Go 27](go/27.md) (linhas ~109-111): padrão container-aware documentado com link oficial; `GOMAXPROCS(0)` usado como leitura, não imposição. |
| GO-02 (P1) | FECHADO | [Go 29](go/29.md) (linhas ~522-551): guarda `len == 0`, tempo de vida na fronteira cgo e link para as regras oficiais; exemplo executado (`total=100`). |
| PY-01 (P1) | FECHADO | [Python 30](python/30.md): objetivo agora diz "estado em memória" + "o que falta para persistência real"; seção lista "as tarefas desaparecem ao reiniciar". Exemplo executado ao vivo via FastAPI (201/404/422). |
| PY-02 (P1) | FECHADO | [Python 31](python/31.md): CLI usa todos os argumentos, cria o checkpoint em runtime, documenta fixtures; [Python 32](python/32.md): CSV inline via `StringIO`, sem arquivo externo; ambos executados (31 com hash cruzado ao `sha256sum`, 32 com pandas). |

Nenhum P0/P1 permanece aberto. Isso zera a fila de bloqueantes conhecida — não equivale ao gate editorial: módulos inteiros (kernel, distribuídos, HPC/GPU, projetos finais) continuam `rascunho` até o percurso completo por volume.

## Remediação executada nesta rodada

| Área | Alteração e evidência | Limite atual |
| --- | --- | --- |
| C, piloto | Aulas 01, 04, 07, 14, 23 e 31 receberam objetivos observáveis, exemplos em `.c`, atividades, pistas e soluções separadas. Os exemplos 01/04/07 e a nova aula 3A compilaram e executaram com GCC e `-Werror`; 14/23/31 compilaram no WSL. O servidor 31 passou por testes de fragmentação, coalescência, entrada inválida, desconexão, leitor lento e 32 clientes; envio parcial forçado e ASan/UBSan também passaram. | Piloto não equivale ao livro completo; o servidor não tem timeout, TLS ou autenticação. O ensaio de leitor lento não força deterministicamente o esgotamento da fila. |
| C, aula 26 | O trecho com `malloc`/`strcpy`/`strcat` agora está identificado como conceitual e inseguro, com riscos de overflow e falha de alocação explícitos. | O builder correto da aula ainda exige verificação de código completo. |
| Rust | Corrigidos contratos de FFI/alocador, ZST em `MeuVec`, capacidade real de `Vec`, vazamentos, overflow e atributos da edição 2024; os exemplos completos das aulas 15 e 16 compilaram e executaram com `rustc 1.95.0`. | Projetos 31/32 com crates externas não foram executados; revisão integral do livro continua. |
| C++ | Aula 11 explicita C++23 para `std::expected`; aula 31 está identificada como esboço local bloqueante e limitada a loopback. Ambas compilaram e tiveram fluxos principais executados. | O servidor ainda não implementa I/O não bloqueante; deve passar pelo laboratório de endurecimento antes de qualquer alegação de escalabilidade. |
| Go e Python | Dez aulas receberam correções de execução, limites de entrada, segurança e precisão técnica. Os exemplos Go relevantes compilaram e os fluxos disponíveis foram executados; Python 25/31 tiveram ensaios de limite, hash, arquivo ausente e timeout. | SUPERADO em 2026-11-17: venv criada no WSL (FastAPI/uvicorn/httpx/pandas instalados); Python 30 executado ao vivo (201/404/422) e 32 executado com pandas; `mmap` Unix de Go segue sem ensaio no Windows nativo (rodar no WSL). |
| Site | Uma prévia Jekyll local apresenta os cinco livros e navega pelo piloto de C. Links, YAML e layout responsivo foram verificados em prévia HTML. | Ruby/Jekyll não estão instalados; ainda não há build real nem publicação do GitHub Pages. |

O verificador de links Markdown fora de blocos de código encontrou todos os destinos locais existentes. A única correspondência residual da busca textual é sintaxe de lambda C++ mostrada como código inline em `rust/11.md`, não um link. O status por aula permanece `rascunho` até revisão curricular e técnica completa.

## Ordem de revisão

1. Corrigir P0 em qualquer idioma imediatamente.
2. Concluir o piloto SDD de C nas aulas 01, 04, 07, 14, 23 e 31.
3. Revisar o livro de C inteiro com a matriz curricular; seguir C++, Rust, Go e Python.
4. Validar fonte executável, links, exercícios, projetos e navegação antes da publicação de cada volume.

Os dois conjuntos de soluções fora de `teoria/` poderão servir como banco de problemas, mas não substituem as atividades e avaliações próprias dos livros. A cobertura de soluções de C e C++ em `Algoritiomos/` ainda não é uniforme; a revisão editorial da masterclass não deve declarar que todos os exercícios estão resolvidos nos cinco idiomas.

## Continuação 3 (2026-11-17) — pontes completas, kernels e revisão parcial

Executado nesta continuação (ambiente Linux/Fedora registrado no CONTINUAR_AQUI): Fases 0–1 (commit inicial, QEMU RISC-V, cross-compilers, Jekyll real), todas as pontes que faltavam nos cinco livros (C: 13b, 23a–23c, 30b, 32a kernel + 32b x86; C++: 08a, 13a, 30a, 30b, 33 kernel + 40 x86; Rust: 12b, 14b, 16b, 23b, 23c, 28a kernel + 28b x86; Go: 08c, 08d, 23b, 28b, 31a; Python: 04b, 14b, 16b, 19a, 23b, 29b, 29c, 33), kernels RISC-V executados nas três linguagens nativas com o mesmo teste de 7 marcadores, revisão integral **parcial e auditada** das aulas originais dos cinco livros (blocos extraídos e compilados; dois defeitos reais corrigidos em C++ 21/24 e um em Rust 16a sob `-O`), e o verificador transversal `verificar_consistencia.py` (232 aulas com linha no CSV, zero links quebrados). `bash teoria/regress.sh`: **64/64** incluindo os testes QEMU e a consistência. **O estado editorial continua `rascunho` em todos os volumes**: o gate da rubrica segue aberto pela separação enunciado/pistas/solução das aulas originais, pela execução dos projetos Cargo 30–32 de Rust e dos projetos Python 30–32 em ambiente limpo, e pela inspeção visual/publicação do site por volume (Fase 7).
