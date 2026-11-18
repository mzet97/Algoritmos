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

## 4. Erros comuns e implicações defensivas

1. **Fuzzar sem invariante:** entradas sem `assert` só acham crash; invariante acha lógica errada que não crasha (linha acima do teto retornada como `Ok`).
2. **Semente que muda por execução:** sem semente fixa, a falha de ontem não reproduz hoje — e relatório irreproduzível é rumor.
3. **Corpus só de válidas:** mutar só o caminho feliz nunca visita o `Erro`. O corpus precisa das três classes desde a semente.
4. **Confundir cobertura de classes com cobertura de código:** as três classes cobertas não provam todos os ramos visitados. Próximo passo honesto: medir com `llvm-cov`, não presumir.
5. **Achar que fuzzing substitui teto:** fuzzer acha onde o teto falha; teto impede o que o fuzzer nem precisa achar. As duas camadas ficam.

## 5. Ponte com as demais trilhas

C cobra o framing com teste de queda real (`SIGKILL`); Rust audita o `unsafe` por invariante; Go exercita o parser com tabela; Python valida entrada com teto antes de `int()`. Em JavaScript, o análogo é fuzzar o parser de query/URL com sementes — o NUL embutido desta aula mora lá também.

## 6. Critério de aprovação e checklist

- [ ] Digo o que cada peça do corpus ensina e qual classe ela visita.
- [ ] Aponto as três invariantes e a entrada que quebraria cada uma.
- [ ] Reproduzo os números executando duas vezes e diffando a saída.
- [ ] Explico onde o libFuzzer entraria (`LLVMFuzzerTestOneInput` no lugar do `main`) e o que ele daria a mais (cobertura guiada).
- [ ] Nomeio o que falta: `llvm-cov` para cobertura medida e TLS para o transporte.

**Referências primárias:** [libFuzzer](https://llvm.org/docs/LibFuzzer.html) para o formato do alvo (o `main` desta aula vira `LLVMFuzzerTestOneInput` sem mudar o parser); [AFL++](https://aflplus.plus/) para mutação guiada por cobertura. Nenhuma afirmação de cobertura de código nesta aula — classes cobertas, ramos presumidos.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
