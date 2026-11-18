---
layout: default
---

# Aula 17 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../17.md#14-exercicios-obrigatorios). Projeto executado nesta rodada:

```go
// bench.go
package bench

import "strings"

func concatenarIngenuo(partes []string) string {
	resultado := ""
	for _, p := range partes {
		resultado += p
	}
	return resultado
}

func concatenarBuilder(partes []string) string {
	var b strings.Builder
	for _, p := range partes {
		b.WriteString(p)
	}
	return b.String()
}

func montarSemReserva(n int) []int {
	var s []int
	for i := 0; i < n; i++ {
		s = append(s, i)
	}
	return s
}

func montarComReserva(n int) []int {
	s := make([]int, 0, n)
	for i := 0; i < n; i++ {
		s = append(s, i)
	}
	return s
}
```

```go
// bench_test.go
package bench

import (
	"strings"
	"testing"
)

var partes = strings.Fields("um dois tres quatro cinco seis sete oito nove dez")

func BenchmarkIngenuo(b *testing.B) {
	for i := 0; i < b.N; i++ {
		concatenarIngenuo(partes)
	}
}

func BenchmarkBuilder(b *testing.B) {
	for i := 0; i < b.N; i++ {
		concatenarBuilder(partes)
	}
}

func BenchmarkSemReserva(b *testing.B) {
	for i := 0; i < b.N; i++ {
		montarSemReserva(1000)
	}
}

func BenchmarkComReserva(b *testing.B) {
	for i := 0; i < b.N; i++ {
		montarComReserva(1000)
	}
}

func TestResultadosIguais(t *testing.T) {
	if concatenarIngenuo(partes) != concatenarBuilder(partes) {
		t.Fatal("concatenacoes divergem")
	}
	if len(montarSemReserva(10)) != len(montarComReserva(10)) {
		t.Fatal("montagens divergem")
	}
}
```

Leitura do `go test -bench=. -benchmem` nesta maquina (rode e anote os seus): ingenua 229 ns/op, 264 B/op, 9 allocs/op; Builder 111 ns/op, 120 B/op, 4 allocs/op; sem reserva 3711 ns/op, 25208 B/op, 12 allocs/op; com reserva 501 ns/op, 0 B/op medidos (o compilador eliminou a alocacao cujo resultado o benchmark descarta — descarte tambem e um dado: guarde o resultado numa variavel de pacote se quiser forcar a alocacao). O numero que importa e a razao entre as versoes na **mesma** execucao, nao o ns/op absoluto — e o teste de igualdade roda antes de qualquer relogio, como manda o protocolo de benchmark das outras trilhas.

5. O roteiro de escape e o da [aula 09](09-solucoes.md): `-gcflags="-m"` e cada "escapes to heap" com a causa nomeada (retorno de endereco, interface, goroutine). Benchmark e escape se conversam: alocacao por operacao alta no `-benchmem` costuma ter uma causa legivel no relatorio de escape.

**Autoverificacao:** teste de igualdade verde antes dos benchmarks, `allocs/op` menor no Builder e na reserva, e um escape do seu projeto explicado pela causa.
