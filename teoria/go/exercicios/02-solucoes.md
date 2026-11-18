---
layout: default
---

# Aula 02 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../02.md#13-exercicios-obrigatorios).

```go
package main

import "fmt"

func classificarSwitch(idade int) string {
	switch {
	case idade < 0:
		return "invalida"
	case idade < 12:
		return "crianca"
	case idade < 18:
		return "adolescente"
	case idade < 60:
		return "adulto"
	default:
		return "idoso"
	}
}

func classificarIf(idade int) string {
	if idade < 0 {
		return "invalida"
	} else if idade < 12 {
		return "crianca"
	} else if idade < 18 {
		return "adolescente"
	} else if idade < 60 {
		return "adulto"
	}
	return "idoso"
}

func main() {
	var declarada int
	atribuida := 42
	fmt.Printf("var nasce %d; := nasce %d\n", declarada, atribuida)
	declarada = 7
	fmt.Printf("var depois: %d\n", declarada)

	var i int
	var b bool
	var s string
	var f float64
	fmt.Printf("zero values: int=%d bool=%v string=%q float=%v\n", i, b, s, f)

	for n := 0; n < 3; n++ {
		fmt.Printf("for classico %d\n", n)
	}
	contador := 0
	for contador < 3 {
		fmt.Printf("for while %d\n", contador)
		contador++
	}

	for _, idade := range []int{8, 15, 30, 70, -1} {
		viaSwitch := classificarSwitch(idade)
		viaIf := classificarIf(idade)
		fmt.Printf("idade %d: %s (if: %s iguais=%v)\n", idade, viaSwitch, viaIf, viaSwitch == viaIf)
		if viaSwitch != viaIf {
			panic("classificacoes divergem")
		}
	}
	fmt.Println("testes ok")
}
```

1. `var declarada int` nasce com o zero value e pode morar fora de funcao; `atribuida := 42` infere o tipo pelo valor, exige atribuicao na declaracao e so existe dentro de funcoes. Na duvida: `:=` para locais com valor obvio, `var` quando o zero value e o ponto ou o tipo precisa ser dito.

3 e 5. O `switch { }` sem expressao avalia cada `case` como condicao booleana na ordem — a mesma cascata do `if/else if`, sem repetir a variavel. A prova de equivalencia aqui e mecanica: as duas classificacoes sao comparadas para cinco idades e o programa entra em `panic` se divergirem.

**Autoverificacao:** zero values `0 false "" 0` impressos sem atribuicao e "iguais=true" em todas as idades.
