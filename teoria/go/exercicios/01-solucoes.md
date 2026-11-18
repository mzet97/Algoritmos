---
layout: default
---

# Aula 01 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../01.md#11-exercicios-obrigatorios).

```go
package main

import "fmt"

const curso = "Algoritmos e Estruturas em Go"

func main() {
	nome := "Ada"
	idade := 36
	cidade := "Recife"

	fmt.Printf("nome: %s\n", nome)
	fmt.Printf("idade: %d\n", idade)
	fmt.Printf("cidade: %s\n", cidade)
	fmt.Printf("curso: %s\n", curso)
}
```

4. Os tres comandos fazem coisas diferentes sobre o mesmo arquivo: `go fmt main.go` reescreve a formatacao (tabs, espacos) sem tocar a logica; `go run main.go` compila para um temporario e executa; `go build -o programa main.go` produz o binario que voce pode rodar de novo sem recompilar. Rodar `go version` primeiro registra a toolchain (a deste ambiente esta em [CONTINUAR_AQUI](../../CONTINUAR_AQUI.md)) — "funciona na minha maquina" sem versao nao e relato reproduzivel.

**Autoverificacao:** as quatro linhas impressas, `gofmt -l` sem saida depois do `go fmt` e o binario do `go build` imprimindo o mesmo que o `go run`.
