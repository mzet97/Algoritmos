---
layout: default
---

# Aula 03 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../03.md#13-exercicios-obrigatorios).

```go
package main

import (
	"errors"
	"fmt"
	"strconv"
	"strings"
)

func multiplicar(a, b int) int {
	return a * b
}

func mensagem(nome string, idade int) string {
	return fmt.Sprintf("%s tem %d anos", nome, idade)
}

func dividir(a, b int) (int, int, error) {
	if b == 0 {
		return 0, 0, errors.New("divisao por zero")
	}
	return a / b, a % b, nil
}

func lerIdade(texto string) (int, error) {
	idade, err := strconv.Atoi(strings.TrimSpace(texto))
	if err != nil {
		return 0, fmt.Errorf("idade %q invalida: %w", texto, err)
	}
	return idade, nil
}

func apresentar(nome, textoIdade string) string {
	idade, err := lerIdade(textoIdade)
	if err != nil {
		return "erro: " + err.Error()
	}
	return mensagem(nome, idade)
}

func main() {
	fmt.Println("multiplicar:", multiplicar(6, 7))

	q, r, err := dividir(17, 5)
	fmt.Printf("17/5: quociente=%d resto=%d erro=%v\n", q, r, err)
	_, _, err = dividir(1, 0)
	fmt.Println("por zero:", err)

	// Retorno ignorado com justificativa: Atoi sobre literal conhecido.
	quarentaEDois, _ := strconv.Atoi("42")
	fmt.Println("ignorado seguro:", quarentaEDois)

	fmt.Println(apresentar("Ada", "36"))
	fmt.Println(apresentar("Beto", "xyz"))
	if multiplicar(6, 7) != 42 || q != 3 || r != 2 {
		panic("resultados inesperados")
	}
	fmt.Println("testes ok")
}
```

4. O `main` grande foi separado em `lerIdade` (entrada e validacao), `mensagem` (formatacao pura) e `apresentar` (orquestracao): cada funcao testavel sem terminal, e o erro ganha contexto com `%w` ao subir, em vez de nascer generico no fundo.

5. O descarte com `_` e seguro no `strconv.Atoi("42")` porque a entrada e um literal conferido por quem escreveu — nao ha mundo exterior que o faca falhar de surpresa. Seria perigoso sobre entrada do usuario: o erro ignorado viraria `0` silencioso e o programa seguiria com um dado inventado. A regra: ignora-se retorno quando a falha e impossivel **por construcao**, nunca por esperanca.

**Autoverificacao:** quociente 3 e resto 2, erro de divisao nomeado e a mensagem de idade invalida carregando o texto original.
