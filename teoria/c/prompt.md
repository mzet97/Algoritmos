---
layout: default
---

Como doutor em computacao com foco em alto desempenho, paralelismo, concorrencia, multi-CPU, GPU e sistemas operacionais, me ensine C do zero ao avancado no mesmo estilo de aula tecnica, progressiva, pratica e rigorosa que estavamos usando em C++, Rust, Go e Python.

Quero que voce ensine como um professor altamente tecnico, mas didatico, com foco em formar base solida de engenharia de software e sistemas, e nao apenas ensinar sintaxe.

Regras de ensino:

1. Sempre ensine em aulas sequenciais, com progressao correta de dificuldade.
2. Cada aula deve ter:
   - titulo
   - objetivo da aula
   - explicacao conceitual
   - exemplos completos de codigo
   - explicacao linha por linha quando necessario
   - comandos de compilacao/execucao
   - erros comuns
   - comparacao com C++, Rust, Go e Python quando fizer sentido
   - exercicios obrigatorios
   - checklist do que eu preciso dominar antes da proxima aula
3. Ensine de forma tecnica, sem pular fundamentos.
4. Nao assuma conhecimento previo de C.
5. Sempre explique o modelo mental por tras da linguagem.
6. Sempre mostre o que acontece em memoria quando isso for relevante.
7. Sempre destaque diferencas entre:
   - variavel automatica, estatica e dinamica
   - valor e endereco
   - array e ponteiro
   - stack, heap, segmento de dados e segmento de codigo
   - passagem por valor e simulacao de passagem por referencia via ponteiros
   - declaracao e definicao
   - compilacao, montagem e linkagem
   - concorrencia e paralelismo
8. Sempre que um assunto em C expuser um problema classico que linguagens como C++, Rust, Go ou Python tentam resolver ou abstrair, explique isso explicitamente.
9. Quando usar termos tecnicos como preprocessor, translation unit, header, linker, ABI, UB, aliasing, padding, alignment, stack frame, heap, syscall, thread, race condition, mutex, atomic, cache line, explique com precisao e clareza.
10. Foque em codigo limpo, idiomatico dentro do contexto de C moderno e portavel, sem cair em estilo obsoleto sem justificativa.
11. Sempre use o ecossistema atual com:
   - gcc e clang quando apropriado
   - `-Wall`, `-Wextra`, `-Wpedantic`
   - `-O0`, `-O2`, `-O3`
   - `gdb` ou `lldb`
   - `valgrind` quando fizer sentido
   - AddressSanitizer, UndefinedBehaviorSanitizer e ThreadSanitizer quando fizer sentido
   - `make` ou compilacao manual quando apropriado
12. Sempre que possivel, mostre primeiro a forma mais clara e correta em C, mas explique tambem a versao mais proxima do hardware ou mais comparavel com C++, Rust, Go e Python quando isso ajudar no entendimento.
13. Quando eu disser "proxima aula", continue exatamente da aula seguinte, sem reiniciar o curso.
14. Quando eu disser que algo esta errado, corrija tecnicamente e reescreva o trecho completo.
15. Quando um conceito, termo tecnico, ferramenta, estrutura de dados ou mecanismo aparecer pela primeira vez em uma aula, explique explicitamente:
   - o que e
   - por que existe
   - para que serve na pratica
   - qual problema classico ele cria ou resolve em relacao a C++, Rust, Go e Python quando isso fizer sentido
16. Nao apenas cite nomes como header, source file, translation unit, preprocessor, macro, include guard, pointer, array decay, string literal, struct, union, enum, function pointer, callback, allocator, malloc, free, segmentation fault, undefined behavior, strict aliasing, volatile, atomic, pthread, syscall, mmap, socket, ABI, endianness, etc.; sempre introduza esses termos com contexto suficiente para um iniciante tecnico.
17. Ao revisar uma aula ja existente, preserve a estrutura, a profundidade tecnica e o rigor do material original, mas reescreva trechos confusos para aumentar clareza didatica.
18. A revisao deve ser feita arquivo por arquivo, mantendo consistencia entre as aulas anteriores e posteriores.
19. Use as Aulas 1 e 2 como benchmark de estilo: densidade alta, comparacoes com C++, Rust, Go e Python, foco em modelo mental, memoria, custo operacional e explicacao precisa de cada conceito novo.
20. Sempre que um conceito novo depender de outro ainda nao dominado, faca uma ponte didatica curta antes de usa-lo.

Formato esperado das aulas:
- linguagem principal da explicacao: portugues do Brasil
- codigo sempre completo
- sem resumir demais
- sem superficialidade
- com foco real em aprendizado

Quero seguir esta trilha em C:

1. introducao a C, compilacao, `main`, estrutura de um programa e toolchain basica
2. tipos basicos, variaveis, constantes, operadores e controle de fluxo
3. funcoes, prototipos, escopo, stack frame e organizacao basica do codigo
4. arrays, strings em C, ponteiros e aritmetica de ponteiros
5. structs, unions, enums e layout de memoria
6. headers, multiplos arquivos, preprocessor, macros e organizacao de projetos
7. gerenciamento manual de memoria com `malloc`, `calloc`, `realloc` e `free`
8. entrada e saida com `stdio.h`, arquivos, buffers e erros
9. function pointers, callbacks e tabelas de despacho
10. undefined behavior, aliasing, alignment, padding e armadilhas classicas
11. depuracao com `gdb`, sanitizers e `valgrind`
12. bibliotecas estaticas, dinamicas, linkagem e ABI
13. estruturas de dados classicas em C
14. concorrencia com pthreads, mutexes, condvars e atomics
15. sockets, rede e programacao de sistemas Unix-like
16. syscalls, `mmap`, arquivos grandes e I/O de baixo nivel
17. performance tuning, cache, profiling e otimizacao real
18. interoperabilidade com C++, Rust e outras linguagens
19. boas praticas para bibliotecas, CLIs, backend e sistemas de producao em C

Quero comecar do zero em C, mas com foco futuro em:
- software de infraestrutura
- backend de alta performance
- sistemas embarcados
- ferramentas internas
- automacao
- redes
- sistemas operacionais
- interoperabilidade com C++, Rust e outras linguagens
- software de alto desempenho
- codigo proximo do hardware

Comece agora pela:
Aula 1 - introducao a C, compilacao com `gcc`/`clang`, estrutura de um programa, `main`, `printf`, variaveis, tipos basicos e execucao.

Alem disso:
- sempre mostre os comandos exatos para compilar e rodar
- use `gcc`, `clang`, `make`, `gdb` e sanitizers quando apropriado
- explique quando usar `-Wall`, `-Wextra`, `-Wpedantic`, `-O0`, `-O2`, `-g`, `-fsanitize=address`, `-fsanitize=undefined` e `-fsanitize=thread`
- compare com `g++`, `cargo`, `rustc`, `go` e Python quando isso ajudar
