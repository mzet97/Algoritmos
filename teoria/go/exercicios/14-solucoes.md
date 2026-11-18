---
layout: default
---

# Aula 14 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../14.md#13-exercicios-obrigatorios). Projeto executado nesta rodada:

```go
// soma.go
package calculadora

// Somar devolve a soma de dois inteiros.
func Somar(a, b int) int {
	return a + b
}

// Max devolve o maior de dois inteiros.
func Max(a, b int) int {
	if a > b {
		return a
	}
	return b
}
```

```go
// soma_test.go
package calculadora

import "testing"

func TestSomar(t *testing.T) {
	if got := Somar(2, 3); got != 5 {
		t.Errorf("Somar(2, 3) = %d, esperado 5", got)
	}
}

func TestMax(t *testing.T) {
	casos := []struct{ a, b, esperado int }{
		{1, 2, 2},
		{9, 4, 9},
		{5, 5, 5},
	}
	for _, c := range casos {
		if got := Max(c.a, c.b); got != c.esperado {
			t.Errorf("Max(%d, %d) = %d, esperado %d", c.a, c.b, got, c.esperado)
		}
	}
}

func BenchmarkSomar(b *testing.B) {
	for i := 0; i < b.N; i++ {
		Somar(2, 3)
	}
}
```

Comandos e leituras verificadas: `go test ./...` verde; `go test -v` nomeando `TestSomar` e `TestMax`; `go test -bench=.` rodando o benchmark com ns/op; `go test -cover` reportando a cobertura do pacote (neste projeto minimo, 100% das poucas linhas — em projetos reais, leia o numero como "por onde passamos", nao como qualidade: 100% de cobertura com asserts fracos continua fraco); `go vet ./...` limpo.

4. Cobertura sem obsessao: o numero baixo aponta codigo nunca executado (pergunte por que); o numero alto nao prova que os testes **conferem** alguma coisa. O par saudavel e cobertura razoavel com testes que falham quando o comportamento quebra — o teste de mesa (`casos`) acima e o formato que escala para isso.

**Autoverificacao:** os tres comandos de teste rodados com saida registrada e um teste seu que falha de proposito uma vez (para provar que ele confere algo) e depois volta a passar.
