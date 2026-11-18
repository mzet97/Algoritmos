---
layout: default
---

# Aula 00: soluções comentadas

Compare após tentar os [exercícios](../00_ponte_python_javascript.md#7-exercícios-obrigatórios). Exemplo em `teoria/go/exemplos/00_ponte.go`; sondas verificadas com `go vet` + `go run`.

1. Previsão: **compila, roda e panic** — `s == nil` é `true`, `len(s)` é `0`, e `s[0] = 1` panic com `index out of range [0] with length 0` (verificado). Declarar não aloca: `var s []int` tem len 0 sem array; `[]int{}` tem len 0 com array. Índice cobra len, não existência — e len 0 nunca tem índice válido. A distinção importa: `append` em nil aloca e funciona; índice em nil (ou vazio) morre. Nil é ausência de array, não array vazio — e Go separa as duas coisas para que "sem dados" (`nil`) e "zero dados" (`[]`) respondam diferente a `== nil` e `json`.

2. Diagnóstico: **erra** (verificado: `Atoi(' 42')` devolve erro). `Atoi` não apara espaço, não ignora sublinhado, não arredonda: formato exato ou erro. "Quase número" é erro porque conversão silenciosa de formato é onde `"$1,000"` vira 1 e `" 42"` vira 42 em produção — explícito aqui significa exato. Quem precisa tolerar formato usa `strings.TrimSpace` antes, declaradamente, não por conta da função.

3. Projeto: `media(nil)` devolve erro (verificado), `media([10 20 30])` devolve `20, nil` (verificado). Divisão por zero disfarçada (`len` no denominador) se previne na porta com `len == 0` — e o erro volta como valor para quem chamar decidir: ignorar com `_`, propagar com `return err`, ou trocar por default. Igual à `dividir` do exemplo: erro é dado, não susto; `panic` seria reservado para bug do programador, não para entrada ruim do usuário.

4. Previsão: leitura devolve o **zero** (`0`, verificado); escrita dá **panic** (`assignment to entry in nil map`, verificado). Ler é seguro porque não muda nada — o mapa nil se comporta como mapa vazio para leitura. Escrever precisa de onde escrever, e nil não é lugar. A assimetria cobra `make` (ou literal) antes do primeiro `=`: mapa se inicializa, não se declara-e-usa. E explica o padrão de código defensivo: função que recebe mapa e escreve deve ou documentar "não-nil" ou inicializar — nunca presumir.
