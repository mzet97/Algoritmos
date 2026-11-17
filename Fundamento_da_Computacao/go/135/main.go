package main

import (
	"fmt"
)

func propriedade() []int {
	resultado := []int{}
	for n := 1000; n < 10000; n++ {
		esq := n / 100
		dir := n % 100
		if (esq+dir)*(esq+dir) == n {
			resultado = append(resultado, n)
		}
	}
	return resultado
}

func main() {
	fmt.Println(propriedade())
}
