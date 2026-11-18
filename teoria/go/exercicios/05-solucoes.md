---
layout: default
---

# Aula 05 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../05.md#12-exercicios-obrigatorios).

```go
package main

import "fmt"

type Livro struct {
	Titulo  string
	Autor   string
	Paginas int
}

type Produto struct {
	Nome  string
	Preco float64
	Qtd   int
}

func main() {
	notas := map[string]int{
		"Ana":  9,
		"Beto": 7,
		"Cida": 10,
	}

	if nota, ok := notas["Ana"]; ok {
		fmt.Printf("Ana: %d (existe=%v)\n", nota, ok)
	}
	nota, ok := notas["Zeca"]
	fmt.Printf("Zeca: %d (existe=%v)\n", nota, ok)

	biblioteca := []Livro{
		{Titulo: "Dom Casmurro", Autor: "Machado de Assis", Paginas: 256},
		{Titulo: "O Cortico", Autor: "Aluisio Azevedo", Paginas: 304},
	}
	for _, livro := range biblioteca {
		fmt.Printf("%s, de %s (%d paginas)\n", livro.Titulo, livro.Autor, livro.Paginas)
	}

	estoque := map[string]Produto{
		"arroz": {Nome: "Arroz 5kg", Preco: 22.90, Qtd: 10},
		"feijao": {Nome: "Feijao 1kg", Preco: 8.50, Qtd: 0},
	}
	arroz := estoque["arroz"]
	arroz.Qtd -= 3
	estoque["arroz"] = arroz
	fmt.Printf("arroz apos venda: %d unidades\n", estoque["arroz"].Qtd)
	if estoque["arroz"].Qtd != 7 || len(notas) != 3 {
		panic("estoque inesperado")
	}
	fmt.Println("testes ok")
}
```

2. Sem o `, ok`, `notas["Zeca"]` devolve `0` — o mesmo valor de um aluno que tirou zero. O padrao `valor, ok` existe para separar "ausente" de "presente com zero", e mapas sao o lugar onde essa distincao mais importa.

5. Mapa guarda **valores**, nao referencias: `estoque["arroz"]` devolve uma copia da struct. Por isso atualizar quantidade pede o ciclo ler→modificar→gravar de volta (ou guardar ponteiros no mapa, com os custos de aliasing que a [aula 06](06-solucoes.md) discute).

**Autoverificacao:** `Zeca: 0 (existe=false)` distingindo ausencia de zero, e o estoque baixando para 7 so com a gravacao de volta.
