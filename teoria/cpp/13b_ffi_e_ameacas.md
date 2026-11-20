---
layout: default
---

# Aula 13b - Fronteira FFI e ameaças formais: exceção não atravessa, dono é único

**Objetivo:** expor uma API C++ como `extern "C"` sem vazar exceção, dono nem invariante — e escrever o modelo de ameaças dos projetos nativos no formato que a rubrica cobra: fronteira, abuso, limite, teste. Pré-requisitos: aulas 04–08 (RAII e ownership), 11 (sanitizers) e 13a (as três falhas clássicas e suas correções). Sem QEMU e sem GPU.

Ao terminar, você deverá dizer, para cada valor que cruza a fronteira, quem valida, quem é dono e o que acontece no abuso — apontando a linha que recusa.

Esta aula é o aprofundamento do módulo CPP-5: a 13a removeu as falhas de dentro; esta fecha a porta de fora. O que falta depois dela: TLS/mTLS (certificado, não token) e o kernel das aulas 33–40, onde a fronteira vira syscall.

## 1. Contrato antes do código

- **Exceção não atravessa:** toda função da fronteira é `noexcept`; `try/catch(...)` interno converte tudo em código (`bad_alloc` vira `-3`). Desenrolar pilha através de C é comportamento indefinido — não "funciona na prática", é proibido pelo padrão.
- **Dono único e opaco:** a fronteira devolve `void*`, nunca o objeto. Quem cria, destrói uma vez; depois de destruir, o cliente zera o handle. A fronteira recusa nulo, mas **não rastreia destruição** — pendurado após destruir é UAF, e o contrato diz isso em vez de fingir que detecta.
- **Tudo que atravessa é validado:** nulo, tamanho zero, acima do teto, índice fora, saída sem espaço — cada um com código próprio. Teto antes de alocar; tamanho antes de copiar.
- **Truncamento nunca é silencioso:** saída curta devolve `-2`, nunca metade copiada. Quem chama decide: chama de novo com buffer maior ou registra o erro.
- **Fora do contrato:** quem garante o tempo de vida do `const char*` de entrada durante a chamada é o chamador; a fronteira copia o que precisa guardar. E handle não é thread-safe por si — concorrência é outro módulo.

## 2. Por que opaco, e não o objeto?

Porque objeto exposto convida o cliente a tocá-lo: copiar, deletar, herdar, ler campo privado por cast. Cada um desses é uma violação de invariante que o autor da classe não consegue impedir depois de publicada. Handle opaco reduz a superfície ao que a fronteira decidiu exportar — e cada decisão de exportar vira validação escrita. O preço é a disciplina do `nullptr` pós-destruição; o prêmio é que o interior continua refatorável sem quebrar ninguém.

## 3. O modelo de ameaças, no formato cobrado

| Fronteira | Abuso | Limite | Teste (linha que recusa) |
|---|---|---|---|
| Exceção C++ → C | `bad_alloc` atravessa `extern "C"` (UB) | `noexcept` + `catch(...)` em toda exportada, código `-3` | caminho que estoura o teto sob `try` |
| Handle opaco | nulo, destruído duas vezes | recusa nulo; destruir exige `nullptr` do cliente | `destruir(nullptr)` recusado; duplo destruir é UAF declarado |
| Tamanho de entrada | gigante que estoura alocação | teto antes de alocar | entrada acima do teto recusada antes do `emplace_back` |
| Índice de leitura | fora da coleção | guarda antes do acesso | índice inválido recusado sem tocar o vetor |
| Buffer de saída | curto demais para o valor | `-2` sem copiar metade | saída curta devolve `-2`, conteúdo intacto |
| Ponteiro de entrada | `const char*` morto no meio da chamada | a fronteira copia o que guarda; tempo de vida é do chamador | documentado, não testável sem UB (como o pendurado) |

A última linha e a do UAF são os dois pontos onde o contrato declara em vez de testar — e a tabela os marca como tal, porque modelo de ameaças honesto distingue "recusado pelo teste" de "recusado pela disciplina do cliente". O teste cobre o que é testável sem UB; o resto é disciplina documentada, e a disciplina está escrita no contrato para que ninguém a confunda com garantia.

## 4. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/13b_ffi.cpp) contém o armazém com a fronteira (`g++` e `clang++`, `-Werror` nos dois):

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror teoria/cpp/exemplos/13b_ffi.cpp -o ffi13b
./ffi13b
clang++ -std=c++20 -Wall -Wextra -Wpedantic -Werror teoria/cpp/exemplos/13b_ffi.cpp -o ffi13b_clang
./ffi13b_clang   # idêntico
```

Com sanitizers (exigido para ponte: ASan/UBSan limpos):

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-omit-frame-pointer teoria/cpp/exemplos/13b_ffi.cpp -o ffi13b_asan
./ffi13b_asan
```

Saída esperada (determinística):

```text
fronteira: nulo, teto e handle invalido recusados
leitura: ok, curto, indice e saida cobertos por codigo
pos-destruir: nulo recusado; pendurado seria UAF (contrato)
13b ok: excecao nao atravessa, dono e unico, abuso vira codigo
```

Um erro real foi evitado durante a escrita: a primeira versão chamava a fronteira com o handle pendurado após destruir e esperava recusa — sob ASan, isso é heap-use-after-free de verdade, não teste. A correção disciplina o cliente (`h = nullptr`) e declara o limite em vez de testá-lo com UB.

### Leitura guiada: quatro linhas, quatro recusas

Cada linha da saída é uma recusa com código, não um "ok": a primeira agrupa três recusas de entrada (nulo, teto, handle inválido) — o teste as agrupa porque as três morrem no mesmo lugar, a validação antes de qualquer uso; a segunda cobre a leitura (ok + curto + índice + saída, os quatro ramos do `ler`); a terceira documenta o limite (nulo recusado de verdade, pendurado declarado); a quarta é o veredito. Repare que nenhum abuso "passa e é tratado depois": a fronteira não tem caminho de recuperação, só de recusa — e é por isso que cada erro tem código próprio em vez de um `-1` genérico: quem chama precisa saber SE aloca mais, desiste ou corrige o índice, e `-1` único não diz nada disso.

## 5. Erros comuns e implicações defensivas

1. **Deixar exceção escapar de `extern "C"`:** o chamador C não tem `catch`; o runtime desenrola para o desconhecido. `noexcept` + `catch(...)` em **toda** função exportada, sem exceção à regra.
2. **Devolver referência ou objeto por valor através da fronteira:** copia com dono ambíguo e destrutor em duas jurisdições. Handle opaco ou nada.
3. **Validar depois de usar:** checar `texto` após `emplace_back` é autópsia. A ordem é nulo → tamanho → teto → cópia.
4. **Truncar em silêncio:** metade copiada com retorno de sucesso corrompe o consumidor calado. `-2` obriga a decisão para cima.
5. **Pro prometer detecção de UAF na fronteira:** sem rastrear destruição, não há como distinguir handle válido de pendurado. Prometer é mentir; o contrato honesto exige o `nullptr` do cliente.
6. **Um `-1` genérico para todos os abusos:** o chamador não sabe se aloca mais, desiste ou corrige o índice. Código por abuso (`-2` curto, `-3` exceção, recusa de nulo) é informação acionável; `-1` único é ruído que finge ser erro.
7. **Guardar o `const char*` em vez de copiar:** se a fronteira retém o ponteiro, o tempo de vida vira aposta no chamador — que pode liberar, reutilizar ou sair de escopo. Copiar o que se guarda transfere a posse junto com os bytes; reter ponteiro alheio é UAF com outro nome.

## 6. Ponte com as demais trilhas

C documenta quem aloca e quem libera na 18; Rust tranca a fronteira no tipo (`unsafe` só dentro do wrapper, erro tipado fora); Go cobra o `C.free` de quem alocou em C; Python isola a extensão atrás de objeto que detém a referência. Em JavaScript, a fronteira é o binding nativo (N-API/wasm): a mesma regra — exceção não atravessa, dono é único, abuso vira código.



**Referencias primarias:** [cppreference, linguagem externa](https://en.cppreference.com/w/cpp/language/language_linkage).

## 7. Critério de aprovação e checklist

- [ ] Marco cada função exportada com `noexcept` e digo para onde vai cada exceção possível.
- [ ] Aponto o dono de cada byte que cruza a fronteira, na ida e na volta.
- [ ] Executo os abusos e digo qual linha recusa cada um, com qual código.
- [ ] Explico por que o pendurado não é detectado e qual disciplina o substitui.
- [ ] Nomeio o que falta: TLS/mTLS, thread-safety declarada e a fronteira-syscall do kernel.

**Referências primárias:** [padrão C++](https://isocpp.org/) § de funções `noexcept` e compatibilidade com C; documentação de [N-API](https://nodejs.org/api/n-api.html) para o mesmo contrato em outro ecossistema. O [6.824 do MIT](https://pdos.csail.mit.edu/6.824/) orienta o nível; nada dele foi copiado.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*

---

## 8. Exercícios obrigatórios

1. (Previsão) Crie com `armazem_criar(0)` e guarde 25 bytes: preveja — `0` ou `-1`? Rode e explique o default (`teto == 0 ? 1024`) — e o risco de um default silencioso numa fronteira que cobra teto.
2. (Diagnóstico) Apague a guarda `h == nullptr` de `armazem_ler` e passe `nullptr`: preveja o sintoma antes de rodar. Rode (sem sanitizer) e explique por que a fronteira checa o que o C++ "garantiria" — nulo atravessa FFI sem cerimônia.
3. (Projeto) Adicione `armazem_tamanho` à fronteira (devolve quantos itens, `-1` em handle nulo, `noexcept` + `try/catch` como as demais). Teste os três casos: vazio, com itens, nulo.
4. (Previsão) Destrua o armazém e use o handle *sem* zerar (apague `h = nullptr`): preveja o que o ASan relata. Rode com sanitizer, transcreva a primeira linha do relatório e explique por que a fronteira recusa nulo mas não rastreia destruição — e de quem é o dever de zerar.

Use [pistas graduais](exercicios/13b_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/13b_solucoes.md) depois de registrar sua própria explicação.

---

### Para se aprofundar

1. Robert Seacord, *Secure Coding in C and C++* (Addison-Wesley, 2. ed., ISBN 978-0321822130); 2. Documentacao do GDB ([sourceware.org/gdb](https://www.sourceware.org/gdb/documentation/)); 3. Documentacao dos sanitizers do Clang ([clang.llvm.org/docs](https://clang.llvm.org/docs/)).
