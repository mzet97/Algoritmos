---
layout: default
---

# Aula 08 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../08.md#13-exercicios-obrigatorios). Estrutura executada nesta rodada:

```text
aula08/
  go.mod            (module exemplo; go 1.x da sua maquina)
  main.go           (package main)
  stringsx/
    stringsx.go     (package stringsx)
```

```go
// stringsx/stringsx.go
package stringsx

import "strings"

// Inverter devolve a string com os caracteres (runes) ao contrario.
func Inverter(s string) string {
	runes := []rune(s)
	for i, j := 0, len(runes)-1; i < j; i, j = i+1, j-1 {
		runes[i], runes[j] = runes[j], runes[i]
	}
	return string(runes)
}

func contarVogais(s string) int {
	total := 0
	for _, r := range strings.ToLower(s) {
		if strings.ContainsRune("aeiou", r) {
			total++
		}
	}
	return total
}

// ContarVogais expoe o contador sem expor a logica interna.
func ContarVogais(s string) int {
	return contarVogais(s)
}
```

```go
// main.go
package main

import (
	"fmt"

	"exemplo/stringsx"
)

func main() {
	fmt.Println(stringsx.Inverter("casa"))
	fmt.Println("vogais em 'algoritmo':", stringsx.ContarVogais("algoritmo"))
}
```

Saida: `asac` e `vogais em 'algoritmo': 4`.

3 e 4. A fronteira de visibilidade em Go e a **primeira letra**: `Inverter` e `ContarVogais` (maiusculas) sao chamados de `main`; `contarVogais` (minuscula) so existe dentro de `stringsx`. A confirmacao do exercicio 4 e um experimento de falha: trocar a chamada para `stringsx.contarVogais(...)` faz o `go build` recusar com `name contarVogais not exported by package stringsx` (mensagem conferida nesta toolchain) — o erro **e** a evidencia, e desfaz-se em seguida. Nao ha `private`/`public`: ha convencao verificada pelo compilador.

1 e 5. `go mod init exemplo` cria o `go.mod` que da nome ao modulo (os imports internos comecam por ele, como `exemplo/stringsx`); `go fmt ./...` e `gofmt` sobre a arvore inteira e `go mod tidy` reconcilia o `go.mod` com os imports reais — os dois rodam limpos neste projeto, e o `go vet ./...` tambem.

**Autoverificacao:** `asac` impresso, o build falhando no simbolo minusculo (e voltando ao normal depois) e `gofmt -l .` sem saida.
