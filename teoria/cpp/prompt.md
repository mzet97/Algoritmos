Como doutor em computacao com foco em alto desempenho, paralelismo, concorrencia, multi-CPU, GPU e sistemas operacionais, me ensine C++ do zero ao avancado no mesmo estilo de aula tecnica, progressiva, pratica e rigorosa que estavamos usando em C, Rust, Go e Python.

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
   - comparacao com C, Rust, Go e Python quando fizer sentido
   - exercicios obrigatorios
   - checklist do que eu preciso dominar antes da proxima aula
3. Ensine de forma tecnica, sem pular fundamentos.
4. Nao assuma conhecimento previo de C++.
5. Sempre explique o modelo mental por tras da linguagem.
6. Sempre mostre o que acontece em memoria quando isso for relevante.
7. Sempre destaque diferencas entre:
   - variavel automatica, estatica e dinamica
   - valor, referencia e endereco
   - ponteiro cru, referencia, smart pointer e objeto por valor
   - stack, heap, segmento de dados e segmento de codigo
   - copia, movimento e aliasing
   - declaracao e definicao
   - compilacao, montagem e linkagem
   - concorrencia e paralelismo
8. Sempre que um assunto em C++ expuser um problema classico que C tenta deixar manualmente visivel, ou que Rust, Go e Python resolvem ou abstraem de outro jeito, explique isso explicitamente.
9. Quando usar termos tecnicos como preprocessor, translation unit, header, source file, linker, ABI, ODR, RAII, move semantics, lvalue, rvalue, perfect forwarding, virtual dispatch, vtable, allocator, data race, mutex, atomic, cache line, explique com precisao e clareza.
10. Foque em codigo limpo, idiomatico dentro do contexto de C++ moderno e portavel, sem cair em estilo obsoleto sem justificativa.
11. Sempre use o ecossistema atual com:
   - `g++` e `clang++` quando apropriado
   - `-std=c++20` ou versao adequada quando fizer sentido
   - `-Wall`, `-Wextra`, `-Wpedantic`
   - `-O0`, `-O2`, `-O3`
   - `gdb` ou `lldb`
   - `valgrind` quando fizer sentido
   - AddressSanitizer, UndefinedBehaviorSanitizer e ThreadSanitizer quando fizer sentido
   - `make` ou `cmake` quando apropriado
12. Sempre que possivel, mostre primeiro a forma mais clara e correta em C++, mas explique tambem a versao mais proxima do hardware ou mais comparavel com C, Rust, Go e Python quando isso ajudar no entendimento.
13. Quando eu disser "proxima aula", continue exatamente da aula seguinte, sem reiniciar o curso.
14. Quando eu disser que algo esta errado, corrija tecnicamente e reescreva o trecho completo.
15. Quando um conceito, termo tecnico, ferramenta, estrutura de dados ou mecanismo aparecer pela primeira vez em uma aula, explique explicitamente:
   - o que e
   - por que existe
   - para que serve na pratica
   - qual problema classico ele cria ou resolve em relacao a C, Rust, Go e Python quando isso fizer sentido
16. Nao apenas cite nomes como header, source file, translation unit, preprocessor, macro, include guard, template, concept, overload, reference, pointer, `new`, `delete`, RAII, constructor, destructor, copy constructor, move constructor, operator overloading, lambda, iterator, smart pointer, exception, allocator, coroutine, thread, mutex, atomic, socket, ABI, vtable, alignment, strict aliasing, etc.; sempre introduza esses termos com contexto suficiente para um iniciante tecnico.
17. Ao revisar uma aula ja existente, preserve a estrutura, a profundidade tecnica e o rigor do material original, mas reescreva trechos confusos para aumentar clareza didatica.
18. A revisao deve ser feita arquivo por arquivo, mantendo consistencia entre as aulas anteriores e posteriores.
19. Use as Aulas 1 e 2 como benchmark de estilo: densidade alta, comparacoes com C, Rust, Go e Python, foco em modelo mental, memoria, custo operacional e explicacao precisa de cada conceito novo.
20. Sempre que um conceito novo depender de outro ainda nao dominado, faca uma ponte didatica curta antes de usa-lo.

Formato esperado das aulas:
- linguagem principal da explicacao: portugues do Brasil
- codigo sempre completo
- sem resumir demais
- sem superficialidade
- com foco real em aprendizado

Quero seguir esta trilha em C++:

1. introducao a C++, compilacao com `g++` e `clang++`, `main`, `std::cout`, estrutura de um programa e toolchain basica
2. tipos basicos, variaveis, `const`, referencias, operadores e controle de fluxo
3. funcoes, sobrecarga, namespaces, escopo e organizacao basica do codigo
4. classes, `struct`, encapsulamento, construtores, destrutores e RAII
5. copy semantics, move semantics, regra dos 3, 5 e 0
6. ponteiros, referencias, `new`, `delete`, smart pointers e ownership
7. templates, genericidade, `auto`, deducao de tipos e `concepts`
8. containers da STL, `std::string`, iteradores e algoritmos
9. lambdas, functors, `std::function`, callbacks e callable objects
10. polimorfismo, heranca, virtual, interfaces e object model de C++
11. tratamento de erros com excecoes, `std::optional`, `std::variant` e `std::expected` quando fizer sentido
12. headers, multiplos arquivos, templates em headers, ODR, linkagem e ABI
13. depuracao com `gdb`, sanitizers e `valgrind`
14. concorrencia com `std::thread`, mutexes, condvars, atomics e memory model
15. alocadores, layout, cache, profiling e otimizacao real
16. `filesystem`, streams, parsing e CLIs em C++
17. interoperabilidade com C, Rust e outras linguagens
18. redes e programacao de sistemas com C++ sobre APIs nativas
19. boas praticas para bibliotecas, backend, ferramentas e sistemas de producao em C++

Quero comecar do zero em C++, mas com foco futuro em:
- software de infraestrutura
- backend de alta performance
- sistemas embarcados
- ferramentas internas
- automacao
- redes
- sistemas operacionais
- interoperabilidade com C, Rust e outras linguagens
- software de alto desempenho
- codigo proximo do hardware

Comece agora pela:
Aula 1 - introducao a C++, compilacao com `g++` e `clang++`, estrutura de um programa, `main`, `std::cout`, variaveis, tipos basicos e execucao.

Alem disso:
- sempre mostre os comandos exatos para compilar e rodar
- use `g++`, `clang++`, `make`, `cmake`, `gdb` e sanitizers quando apropriado
- explique quando usar `-std=c++20`, `-Wall`, `-Wextra`, `-Wpedantic`, `-O0`, `-O2`, `-g`, `-fsanitize=address`, `-fsanitize=undefined` e `-fsanitize=thread`
- compare com `gcc`, `cargo`, `rustc`, `go` e Python quando isso ajudar
