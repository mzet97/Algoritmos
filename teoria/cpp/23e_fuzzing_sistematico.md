---
layout: default
---

# Aula 23e - Fuzzing sistemático: o parser sob mutação determinística

**Objetivo:** fuzzar o parser de framing da 23a (linha com teto, NUL rejeitado, resto incompleto) com um mutador de semente fixa: 200 mil entradas reproduzíveis, invariantes cobradas a cada uma, memória sob ASan/UBSan. Pré-requisitos: aulas 13a (sanitizers), 23a (o protocolo) e 23d (os tetos). Linux ou WSL2. Sem QEMU e sem GPU.

Ao terminar, você deverá dizer o que o corpus cobre, o que cada invariante proíbe e por que a mesma semente sempre dá o mesmo veredito — e onde o libFuzzer entraria se o toolchain o tivesse.

Esta aula é o módulo CPP-7 na parte de fuzzing: a 23a endureceu o servidor, a 23d orçou o servidor, esta interroga o parser. O que falta depois dela: TLS/autenticação (certificado, não token).

## 1. Contrato antes do código

- **Alvo:** `parse_frame` puro — sem socket, sem estado. Fuzzar o loop inteiro mistura protocolo com I/O; fuzzar o parser isola a decisão.
- **Mutador determinístico:** xorshift64 com semente fixa sobre um corpus de 3 sementes e 6 peças (válida, acima do teto, NUL, incompleta, gigante). Mesma semente, mesma sequência, mesmo relatório — reproduzível por construção.
- **Invariantes por entrada:** `Ok` nunca acima do teto e sem NUL; `Incompleta` sempre dentro do teto total; determinismo cobrado reexecutando cada entrada.
- **Memória pelo sanitizer:** o `assert` cobra lógica; ASan/UBSan cobram o que a lógica não vê (leitura além do vetor, overflow de `size_t`).
- **Fora do contrato:** libFuzzer/AFL (este toolchain não tem o sanitizer de fuzzing — declarado, com a porta de entrada escrita abaixo) e minimização automática de corpus.

## 2. Por que determinístico, e não "aleatório de verdade"?

Fuzzing aleatório acha; fuzzing determinístico prova que continua achando o mesmo. Com semente fixa, cada execução é regressão: se uma mudança futura introduzir violação, o relatório muda no mesmo ponto e o `git bisect` encontra o culpado. Aleatoriedade sem semente é loteria; com semente, é teste.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/23e_fuzz.cpp) contém o fuzzer (`clang++` com sanitizers; `g++` compila igual):

```bash
clang++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-omit-frame-pointer teoria/cpp/exemplos/23e_fuzz.cpp -o fuzz23e
./fuzz23e
g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror teoria/cpp/exemplos/23e_fuzz.cpp -o fuzz23e_gpp
./fuzz23e_gpp   # mesma contagem, mesma semente
```

Saída esperada (determinística — estes números fazem parte do contrato):

```text
fuzz: 200000 entradas (ok=47063 incompletas=85153 erros=67784)
classes cobertas: Ok, Incompleta, Erro — 0 violacoes
23e ok: parser sob mutacao deterministica, memoria sob sanitizer
```

Um erro real foi pego pelo `-Werror` no caminho: variável de contagem escrita e nunca lida (`unused-but-set-variable`). O compilador acusou antes do fuzzer precisar — camadas diferentes pegam bugs diferentes, e é por isso que as duas rodam.

### Leitura guiada: o que os números dizem sobre o mutador

`ok=47063 incompletas=85153 erros=67784` não é placar — é o retrato do mutador: as três classes aparecem em proporção comparável, o que prova que a mutação visita o parser inteiro em vez de ficar presa no caminho feliz. Se `erros` fosse 0, o mutador nunca geraria NUL nem estouro — e as invariantes de `Erro` seriam vacinas sem doença. Se `ok` fosse 0, o corpus não teria semente válida e o `Incompleta` nunca seria distinguido do `Erro`. A invariante "determinismo cobrado reexecutando cada entrada" fecha o círculo: o mesmo número, duas vezes, prova que a contagem é propriedade do par (corpus, semente), não do relógio. Por isso os números fazem parte do contrato — mudaram, algo mudou (no parser, no mutador ou na semente), e o `git bisect` acha o quê.

## 4. Erros comuns e implicações defensivas

1. **Fuzzar sem invariante:** entradas sem `assert` só acham crash; invariante acha lógica errada que não crasha (linha acima do teto retornada como `Ok`).
2. **Semente que muda por execução:** sem semente fixa, a falha de ontem não reproduz hoje — e relatório irreproduzível é rumor.
3. **Corpus só de válidas:** mutar só o caminho feliz nunca visita o `Erro`. O corpus precisa das três classes desde a semente.
4. **Confundir cobertura de classes com cobertura de código:** as três classes cobertas não provam todos os ramos visitados. Próximo passo honesto: medir com `llvm-cov`, não presumir.
5. **Achar que fuzzing substitui teto:** fuzzer acha onde o teto falha; teto impede o que o fuzzer nem precisa achar. As duas camadas ficam.
6. **Mutar bytes sem mutar comprimentos:** o mutador que só troca conteúdo nunca gera "acima do teto" nem "incompleta" — as duas classes que vivem na fronteira do tamanho. Peça gigante e peça cortada existem no corpus exatamente porque tamanho é dimensão de mutação, não detalhe.
7. **Cobrar determinismo sem reexecutar:** afirmar "mesma semente, mesmo resultado" sem rodar duas vezes é promessa. A reexecução por entrada custa o dobro e compra a única prova que importa: a de que o relatório de ontem vale hoje.

## 5. Ponte com as demais trilhas

C cobra o framing com teste de queda real (`SIGKILL`); Rust audita o `unsafe` por invariante; Go exercita o parser com tabela; Python valida entrada com teto antes de `int()`. Em JavaScript, o análogo é fuzzar o parser de query/URL com sementes — o NUL embutido desta aula mora lá também.



**Referencias primarias:** [libFuzzer](https://llvm.org/docs/LibFuzzer.html); [AFL++](https://aflplus.plus/); [The Fuzzing Book](https://www.fuzzingbook.org/).

## 6. Critério de aprovação e checklist

- [ ] Digo o que cada peça do corpus ensina e qual classe ela visita.
- [ ] Aponto as três invariantes e a entrada que quebraria cada uma.
- [ ] Reproduzo os números executando duas vezes e diffando a saída.
- [ ] Explico onde o libFuzzer entraria (`LLVMFuzzerTestOneInput` no lugar do `main`) e o que ele daria a mais (cobertura guiada).
- [ ] Nomeio o que falta: `llvm-cov` para cobertura medida e TLS para o transporte.

**Referências primárias:** [libFuzzer](https://llvm.org/docs/LibFuzzer.html) para o formato do alvo (o `main` desta aula vira `LLVMFuzzerTestOneInput` sem mudar o parser); [AFL++](https://aflplus.plus/) para mutação guiada por cobertura. Nenhuma afirmação de cobertura de código nesta aula — classes cobertas, ramos presumidos.

## 7. Exercícios obrigatórios

1. (Previsão) Troque a semente (`estado` inicial) e rode: preveja — as contagens mudam, mas as 3 classes continuam cobertas? Rode com 2 sementes e explique o que é determinístico (sequência por semente) e o que é cobertura (mutador fechado em 6 peças).
2. (Diagnóstico) Apague a checagem de NUL embutido: preveja qual `assert` cai. Rode sob sanitizer e explique por que a invariante que morde (`linha sem NUL`) mora no teste, não no parser — e onde o NUL faria estrago depois (todo `c_str` dali em diante).
3. (Projeto) Prenda o off-by-one do teto: construa a linha de 4095 + `\n` (máxima válida) e a de 4096 + `\n` (mínima inválida) e prove `Ok`/`Erro` lado a lado. Explique a condição `i + 1 > LINHA_MAX`: o teto conta o `\n` junto — e linha "de 4096" com newline já é 4097.
4. (Previsão) Dobre `ITERACOES` para 400 mil: preveja tempo (linear?) e se alguma classe nova aparece. Rode e explique por que mais iteração do mesmo mutador não é mais cobertura — espaço saturado pede peça nova, não mais sorteio.

Use [pistas graduais](exercicios/23e_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/23e_solucoes.md) depois de registrar sua própria explicação.

---

### Para se aprofundar

1. W. Richard Stevens, Bill Fenner e Andrew Rudoff, *UNIX Network Programming, vol. 1* (Addison-Wesley, 3. ed., ISBN 978-0131411555); 2. Martin Kleppmann, *Designing Data-Intensive Applications* (O'Reilly, ISBN 978-1449373320), caps. 4-5; 3. Bjarne Stroustrup, *A C++ Programming Language* (Addison-Wesley, 4. ed., ISBN 978-0321563842).
