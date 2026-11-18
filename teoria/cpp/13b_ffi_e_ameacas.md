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

## 3. Exemplo completo e executável

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

## 4. Erros comuns e implicações defensivas

1. **Deixar exceção escapar de `extern "C"`:** o chamador C não tem `catch`; o runtime desenrola para o desconhecido. `noexcept` + `catch(...)` em **toda** função exportada, sem exceção à regra.
2. **Devolver referência ou objeto por valor através da fronteira:** copia com dono ambíguo e destrutor em duas jurisdições. Handle opaco ou nada.
3. **Validar depois de usar:** checar `texto` após `emplace_back` é autópsia. A ordem é nulo → tamanho → teto → cópia.
4. **Truncar em silêncio:** metade copiada com retorno de sucesso corrompe o consumidor calado. `-2` obriga a decisão para cima.
5. **Pro prometer detecção de UAF na fronteira:** sem rastrear destruição, não há como distinguir handle válido de pendurado. Prometer é mentir; o contrato honesto exige o `nullptr` do cliente.

## 5. Ponte com as demais trilhas

C documenta quem aloca e quem libera na 18; Rust tranca a fronteira no tipo (`unsafe` só dentro do wrapper, erro tipado fora); Go cobra o `C.free` de quem alocou em C; Python isola a extensão atrás de objeto que detém a referência. Em JavaScript, a fronteira é o binding nativo (N-API/wasm): a mesma regra — exceção não atravessa, dono é único, abuso vira código.

## 6. Critério de aprovação e checklist

- [ ] Marco cada função exportada com `noexcept` e digo para onde vai cada exceção possível.
- [ ] Aponto o dono de cada byte que cruza a fronteira, na ida e na volta.
- [ ] Executo os abusos e digo qual linha recusa cada um, com qual código.
- [ ] Explico por que o pendurado não é detectado e qual disciplina o substitui.
- [ ] Nomeio o que falta: TLS/mTLS, thread-safety declarada e a fronteira-syscall do kernel.

**Referências primárias:** [padrão C++](https://isocpp.org/) § de funções `noexcept` e compatibilidade com C; documentação de [N-API](https://nodejs.org/api/n-api.html) para o mesmo contrato em outro ecossistema. O [6.824 do MIT](https://pdos.csail.mit.edu/6.824/) orienta o nível; nada dele foi copiado.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
