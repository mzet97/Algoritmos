---
layout: default
---

# Aula 15 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../15.md#14-exercicios-obrigatorios).

```go
package main

import (
	"context"
	"errors"
	"fmt"
	"time"
)

func trabalhar(ctx context.Context, nome string) error {
	for i := 0; i < 10; i++ {
		select {
		case <-ctx.Done():
			fmt.Printf("%s interrompido na etapa %d\n", nome, i)
			return ctx.Err()
		case <-time.After(30 * time.Millisecond):
			// uma etapa de trabalho
		}
	}
	fmt.Println(nome, "terminou todas as etapas")
	return nil
}

func main() {
	ctxCancel, cancel := context.WithCancel(context.Background())
	go func() {
		time.Sleep(80 * time.Millisecond)
		cancel()
	}()
	err := trabalhar(ctxCancel, "com WithCancel")
	fmt.Println("erro devolvido:", err, "| e cancelamento?", errors.Is(err, context.Canceled))

	ctxTimeout, cancel2 := context.WithTimeout(context.Background(), 70*time.Millisecond)
	defer cancel2()
	err = trabalhar(ctxTimeout, "com WithTimeout")
	fmt.Println("erro devolvido:", err, "| e prazo?", errors.Is(err, context.DeadlineExceeded))
	fmt.Println("testes ok")
}
```

Cada trabalhador para no meio (etapa 2–3 de 10) e devolve o erro do contexto: `context.Canceled` no cancelamento manual, `context.DeadlineExceeded` no timeout. Devolver esse erro, em vez de um generico, e o que permite a quem chamou decidir (tentar de novo? desistir? registrar como cliente que sumiu?).

4. A convencao e mecanica: `func F(ctx context.Context, ...)` — primeiro parametro, nunca campo de struct (o contexto e da **chamada**, nao do objeto), nunca `nil` (use `context.TODO()` no rascunho e `Background()` na raiz).

5. Cenario operacional: o cliente fecha a aba aos 200 ms, mas o handler sem contexto continua consultando o banco por 8 segundos e segurando uma conexao do pool. Sob carga, o pool inteiro fica ocupado com trabalho para clientes que ja foram embora — latencia sobe para os clientes vivos e o sintoma aparece longe da causa. Propagar `ctx` ate a consulta e o que transforma desconexao em liberacao.

**Autoverificacao:** os dois erros distinguidos por `errors.Is`, nenhum contexto guardado em struct e o trabalhador parando antes das 10 etapas nos dois casos.
