---
layout: default
---

# Aula 23e: pistas graduais

Volte aos [exercícios](../23e_fuzzing_sistematico.md#7-exercícios-obrigatórios) antes de abrir as soluções.

1. Semente nova = sequência nova, mas o mesmo mutador com as mesmas 6 peças: as proporções mudam, as classes permanecem (o `assert` final cobra as 3). Determinismo é "mesma semente, mesma sequência" — cobertura é "peças que alcançam as classes". Uma não implica a outra: semente fixa sem peça de NUL jamais acharia o NUL.
2. Sem a checagem, a peça `{'A', 0, 'B', '\n'}` vira `Ok` com NUL dentro — e o `assert(r.linha.find('\0') == npos)` cai. O parser "passou" e o invariante do protocolo não: NUL em `std::string` sobrevive até o primeiro `c_str()`, onde a linha é truncada em silêncio. A invariante mora no teste porque é do *protocolo*, não da função.
3. `i` é o índice do `\n`: `i + 1` inclui o newline na conta. 4095 + `\n` → `i + 1 = 4096`, passa; 4096 + `\n` → 4097, cai. "Linha de 4 KiB" com terminador já é 4097 bytes no fio — teto que não conta o delimitador aceita 1 a mais, sempre.
4. Tempo dobra (linear no número de entradas), classes continuam as mesmas 3: o mutador só combina 6 peças de 4 jeitos — 400 mil sorteios revisitam o mesmo espaço. Cobertura cresce com peça nova (7ª peça = 7ª ideia de abuso), não com iteração. Fuzz sem peça nova é treino sem adversário novo.
