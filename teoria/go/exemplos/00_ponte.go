package main

import (
	"fmt"
	"strconv"
)

func dividir(a, b int) (int, error) {
	if b == 0 {
		return 0, fmt.Errorf("divisao por zero")
	}
	return a / b, nil
}

func main() {
	// Zero de cada tipo: declarar e ler, sem atribuir.
	var inteiro int
	var texto string
	var condicao bool
	fmt.Println("zeros:", inteiro, texto == "", condicao)

	// Sem coercao: a conversao e explicita e devolve erro.
	numero, err := strconv.Atoi("42")
	if err != nil {
		panic(err)
	}
	_, errInvalido := strconv.Atoi("quarenta")
	if errInvalido == nil {
		panic("texto deveria falhar")
	}
	fmt.Println("conversao explicita:", numero, "| texto invalido recusado:", errInvalido != nil)

	// Slice: atribuir apelida; copy independe.
	original := []int{1, 2, 3}
	apelido := original
	apelido[0] = 99
	independente := make([]int, len(original))
	copy(independente, original)
	independente[0] = 1
	fmt.Println("apelido compartilha:", original[0] == 99,
		"| copia independe:", independente[0] == 1 && original[0] == 99)

	// Erro como valor: quem chama decide, na hora.
	if q, err := dividir(10, 2); err != nil {
		panic(err)
	} else {
		fmt.Println("10/2 =", q)
	}
	if _, err := dividir(10, 0); err == nil {
		panic("divisao por zero deveria falhar")
	}
	fmt.Println("testes ok")
}
