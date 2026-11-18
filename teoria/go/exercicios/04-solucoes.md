---
layout: default
---

# Aula 04 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../04.md#12-exercicios-obrigatorios).

```go
package main

import "fmt"

func main() {
	arr := [5]int{10, 20, 30, 40, 50}
	for i, v := range arr {
		fmt.Printf("arr[%d]=%d ", i, v)
	}
	fmt.Println()

	fatia := arr[1:4]
	fatia[0] = 99
	fmt.Printf("slice mexeu no array: arr[1]=%d\n", arr[1])

	numeros := []int{1, 2}
	numeros = append(numeros, 3, 4)
	fmt.Printf("append: %v len=%d cap=%d\n", numeros, len(numeros), cap(numeros))

	ascii := "casa"
	acentuada := "casaé"
	fmt.Printf("bytes: %q=%d %q=%d\n", ascii, len(ascii), acentuada, len(acentuada))

	for i, r := range "olá" {
		fmt.Printf("rune %d: %c (byte %d)\n", i, r, i)
	}
	runes := []rune("olá")
	fmt.Printf("runes em 'olá': %d (bytes: %d)\n", len(runes), len("olá"))
	if len(runes) != 3 || len("olá") != 4 {
		panic("contagens inesperadas")
	}
	fmt.Println("testes ok")
}
```

3. `append` devolve o slice que pode ter mudado de endereco: descartar o retorno e o bug classico — quando a capacidade estoura, o slice novo mora em outro array e o antigo fica com os dados velhos. 4 e 5 sao o mesmo fato visto de dois lados: `len("olá")` e 4 (bytes UTF-8), mas ha 3 caracteres; `[]rune` paga uma conversao para contar e indexar por caractere. O indice do `range` sobre string tambem e de byte (0, 1, 3 em `"olá"`), nao de caractere.

**Autoverificacao:** `arr[1]=99` depois de escrever pelo slice, runes 3 contra bytes 4 e nenhum retorno de `append` descartado.
