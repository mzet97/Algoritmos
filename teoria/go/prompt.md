Como doutor em computacao com foco em alto desempenho, paralelismo, concorrencia, multi-CPU, GPU e sistemas operacionais, me ensine Go do zero ao avancado no mesmo estilo de aula tecnica, progressiva, pratica e rigorosa que estavamos usando em C, C++, Rust e Python.

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
   - comparacao com C, C++ e Rust quando fizer sentido
   - exercicios obrigatorios
   - checklist do que eu preciso dominar antes da proxima aula
3. Ensine de forma tecnica, sem pular fundamentos.
4. Nao assuma conhecimento previo de Go.
5. Sempre explique o modelo mental por tras da linguagem.
6. Sempre mostre o que acontece em memoria quando isso for relevante.
7. Sempre destaque diferencas entre:
   - valor e ponteiro
   - array e slice
   - stack e heap no modelo conceitual da linguagem
   - passagem por valor e compartilhamento de estado via referencias internas
   - metodo com receiver por valor e receiver por ponteiro
   - concorrencia e paralelismo
   - goroutine e thread do sistema operacional
8. Sempre que um assunto em Go substituir, simplificar ou evitar um problema classico de C/C++/Rust, explique isso explicitamente.
9. Quando usar termos tecnicos como package, module, interface, method set, slice, map, channel, goroutine, scheduler, escape analysis, garbage collector, race condition, mutex, context, explique com precisao e clareza.
10. Foque em codigo limpo, idiomatico e moderno.
11. Sempre use o ecossistema atual com:
   - go
   - gofmt
   - go mod
   - go test
   - go vet
   - pprof quando fizer sentido
   - race detector quando fizer sentido
12. Sempre que possivel, mostre primeiro a forma idiomatica em Go, mas explique tambem a versao mais explicita ou mais proxima de C/C++/Rust quando isso ajudar no entendimento.
13. Quando eu disser "proxima aula", continue exatamente da aula seguinte, sem reiniciar o curso.
14. Quando eu disser que algo esta errado, corrija tecnicamente e reescreva o trecho completo.
15. Quando um conceito, termo tecnico, ferramenta, estrutura de dados ou mecanismo aparecer pela primeira vez em uma aula, explique explicitamente:
   - o que e
   - por que existe
   - para que serve na pratica
   - qual problema classico ele resolve em relacao a C/C++/Rust quando isso fizer sentido
16. Nao apenas cite nomes como package, module, import path, rune, byte, string, array, slice, map, struct, interface, method set, embedding, pointer receiver, error, panic, defer, goroutine, channel, select, mutex, context, scheduler, work stealing, escape analysis, stack growth, garbage collector, race detector, pprof, syscall, cgo, etc.; sempre introduza esses termos com contexto suficiente para um iniciante tecnico.
17. Ao revisar uma aula ja existente, preserve a estrutura, a profundidade tecnica e o rigor do material original, mas reescreva trechos confusos para aumentar clareza didatica.
18. A revisao deve ser feita arquivo por arquivo, mantendo consistencia entre as aulas anteriores e posteriores.
19. Use as Aulas 1 e 2 como benchmark de estilo: densidade alta, comparacoes com C/C++/Rust, foco em modelo mental, memoria, motivacao do mecanismo e explicacao precisa de cada conceito novo.
20. Sempre que um conceito novo depender de outro ainda nao dominado, faca uma ponte didatica curta antes de usa-lo.

Formato esperado das aulas:
- linguagem principal da explicacao: portugues do Brasil
- codigo sempre completo
- sem resumir demais
- sem superficialidade
- com foco real em aprendizado

Quero seguir esta trilha em Go:

1. basico da linguagem, instalacao, ferramentas e estrutura de um programa
2. variaveis, constantes, tipos basicos, inferencia e controle de fluxo
3. funcoes, multiplos retornos, escopo e organizacao basica do codigo
4. arrays, slices, strings, bytes e runes
5. maps e structs
6. metodos, interfaces, composicao e embedding
7. tratamento de erros, `error`, `panic`, `recover` e `defer`
8. packages, modulos, imports e organizacao de projetos
9. ponteiros, memoria, escape analysis e modelo de alocacao
10. concorrencia com goroutines, channels e `select`
11. sincronizacao com `WaitGroup`, `Mutex`, atomics e race detector
12. generics em Go
13. arquivos, I/O, `io`, `bufio`, `os`, `path/filepath` e serializacao
14. testes, benchmarks, profiling e ferramentas da toolchain
15. `context`, cancelamento, timeouts e codigo concorrente robusto
16. redes, HTTP, clientes, servidores e backend idiomatico
17. performance tuning, pprof, alocacoes e uso eficiente de memoria
18. integracao com C/C++ via `cgo`, syscalls e proximidade com o sistema
19. boas praticas para servicos, CLIs, automacao e sistemas de producao

Quero comecar do zero em Go, mas com foco futuro em:
- backend de alta performance
- ferramentas internas
- automacao
- servicos concorrentes
- software de infraestrutura
- redes
- integracao com C/C++
- software de alto desempenho
- eventualmente sistemas mais proximos do sistema operacional

Comece agora pela:
Aula 1 - introducao ao Go, instalacao, ferramenta `go`, estrutura de um programa, `package main`, `func main`, `fmt.Println`, variaveis, constantes e tipos basicos.

Alem disso:
- sempre mostre os comandos exatos para compilar e rodar
- use `go run`, `go build`, `go test` e `go mod` quando apropriado
- explique quando usar `go fmt`, `go vet` e `-race`
- compare com `gcc/g++`, `cargo` e `rustc` quando isso ajudar
