---
layout: default
---

# Aula 06: pistas graduais

Volte aos [exercicios](../06.md#16-exercicios-obrigatorios) antes de abrir as solucoes.

1. O header declara, o `.c` define: `util.h` guarda prototipos e a guarda de inclusao; `main.c` so inclui e chama.
2. Na macro, parenese cada parametro e a expressao inteira; depois teste com uma expressao como `MAXIMO(a + 1, b)`.
3. A funcao interna nao aparece no header e e declarada com `static`; a publica aparece nos dois lugares.
4. Compile com `-c` gerando um `.o` por fonte e so entao linke os objetos; observe qual etapa falha quando um prototipo some.
5. Para a explicacao: siga um `#include "util.h"` como substituicao de texto feita antes da compilacao e relacione com a guarda de inclusao.
