---
layout: default
---

# Aula 23e: soluções comentadas

Compare após tentar os [exercícios](../23e_fuzzing_sistematico.md#7-exercícios-obrigatórios). Experimentos sobre `teoria/cpp/exemplos/23e_fuzz.cpp` com `clang++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-omit-frame-pointer`.

1. Previsão: com semente `0xDEADBEEF`, as contagens mudam (`ok=46444` contra `47063` da semente original) mas as 3 classes continuam cobertas — o `assert` final passa (verificado). Determinístico = mesma semente, mesma sequência, mesmas contagens; cobertura = peças que alcançam as classes. A semente nova prova as duas coisas de uma vez: sequência diferente, mesmas 3 classes — porque a cobertura mora nas 6 peças, não no número mágico.

2. Diagnóstico: cai o `assert(r.linha.find('\0') == std::string::npos)` (verificado). Sem a checagem, a peça `{'A', 0, 'B', '\n'}` vira `Ok` com NUL dentro da `linha`. O parser "passou"; o protocolo, não: o `std::string` carrega o NUL até o primeiro `c_str()`, `printf("%s")` ou comparação com literal — onde a linha é truncada em silêncio e `"AB"` passa a significar `"AB\0CD"`. A invariante mora no teste porque é do protocolo (linha é texto sem NUL), não da função (que só fatia bytes).

3. Projeto: `4095 + '\n'` → `Ok (0)`; `4096 + '\n'` → `Erro (2)` (verificado lado a lado). `i` é o índice do `'\n'`; `i + 1 > LINHA_MAX` conta o newline no teto: 4095 + 1 = 4096 passa no limite, 4096 + 1 = 4097 cai. "Linha de 4 KiB" com terminador ocupa 4097 bytes no fio — teto que conta só o conteúdo aceita 1 a mais, sempre. O par máxima-válida/mínima-inválida é o que prende o off-by-one: um lado só prova metade.

4. Previsão: as classes continuam as mesmas 3 (`ok/incompletas/erros` só dobram de volume: 93549/171071/135380 em 400 mil) e o tempo acompanha as iterações (verificado: dezenas de segundos nos dois regimes, mesma ordem por entrada). Nada de novo aparece — o mutador combina 6 peças de 4 jeitos, e 400 mil sorteios revisitam o mesmo espaço saturado. Cobertura cresce com peça nova (a 7ª peça do exercício 3, por exemplo), não com iteração: fuzz sem ideia nova de abuso é treino sem adversário novo.
