---
layout: default
---

# Aula 00: pistas graduais

Volte aos [exercícios](../00_ponte_python_javascript.md#7-exercícios-obrigatórios) antes de abrir as soluções.

1. `s == nil` dá true e `len(s)` dá 0 — declarar não aloca. O índice `s[0]` panic (`index out of range [0] with length 0`, verificado). Nil é ausência de array, não array vazio: `var s []int` tem len 0 sem array; `[]int{}` tem len 0 com array. Índice cobra len, não existência — e len 0 nunca tem índice 0.
2. `Atoi(" 42")` erra (verificado). `Atoi` não apara, não ignora, não arredonda: formato exato ou erro. "Quase número" é erro porque conversão silenciosa de formato é onde `"$1,000"` vira 1 — explícito aqui significa exato, não aproximado.
3. `media(nil)` devolve erro (verificado), `media([10 20 30])` devolve 20 (verificado). Divisão por zero disfarçada (`len` no denominador) se previne na porta com `len == 0` — e o erro volta como valor, não `panic`, para quem chamar decidir: ignorar, propagar ou trocar por default. Igual à `dividir` do exemplo: erro é dado, não susto.
4. Leitura em mapa nil devolve o zero (`0`, verificado); escrita panic (`assignment to entry in nil map`, verificado). Ler é seguro porque não muda nada; escrever precisa de onde escrever — e nil não é lugar. A assimetria cobra `make` antes do primeiro `=`: mapa se inicializa, não se declara-e-usa.
