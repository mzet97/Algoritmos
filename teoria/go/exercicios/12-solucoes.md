---
layout: default
---

# Aula 12 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../12.md#13-exercicios-obrigatorios).

```go
package main

import "fmt"

func Primeiro[T any](s []T) (T, bool) {
	if len(s) == 0 {
		var zero T
		return zero, false
	}
	return s[0], true
}

type Numerico interface {
	~int | ~int64 | ~float64
}

func Max[T Numerico](a, b T) T {
	if a > b {
		return a
	}
	return b
}

func Soma[T Numerico](s []T) T {
	var total T
	for _, v := range s {
		total += v
	}
	return total
}

type Par[T any] struct {
	Primeiro, Segundo T
}

func main() {
	if v, ok := Primeiro([]string{"a", "b"}); ok {
		fmt.Println("primeiro:", v)
	}
	if _, ok := Primeiro([]int{}); !ok {
		fmt.Println("slice vazio: sem primeiro")
	}
	fmt.Println("max int:", Max(3, 9))
	fmt.Println("max float:", Max(2.5, 9.25))
	fmt.Println("soma:", Soma([]int{1, 2, 3, 4}))

	par := Par[string]{Primeiro: "esquerda", Segundo: "direita"}
	fmt.Printf("par: %s/%s\n", par.Primeiro, par.Segundo)
	if Max(3, 9) != 9 || Soma([]int{1, 2, 3, 4}) != 10 {
		panic("genericos divergiram")
	}
	fmt.Println("testes ok")
}
```

4. Resolva com **interface** quando o conjunto de tipos e aberto e o comportamento e o que importa (qualquer `Falante` da [aula 06](06-solucoes.md), inclusive tipos que voce nao controla). Resolva com **generic** quando o algoritmo e o mesmo para tipos fechados e voce quer o tipo exato de volta sem conversao (`Max` devolve `int` para ints, nao `any`). Generic nao e "interface moderna": um apaga o tipo na execucao, o outro o multiplica na compilacao.

5. O utilitario repetido que vira generico sem arrependimento e o que difere **so no tipo** (buscar, somar, primeiro). Se as copias diferem em comportamento (formatacao, validacao), generic com restricoes crescentes vira uma interface pior disfarcada.

**Autoverificacao:** vazio devolve `ok=false` sem panic, `Max` com int e float no mesmo fonte e um exemplo seu de cada lado da fronteira interface/generic.
