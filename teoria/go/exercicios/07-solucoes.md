---
layout: default
---

# Aula 07 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../07.md#13-exercicios-obrigatorios).

```go
package main

import (
	"errors"
	"fmt"
)

var ErrNegativo = errors.New("numero negativo")

func raizAproximada(n int) (int, error) {
	if n < 0 {
		return 0, fmt.Errorf("raiz de %d: %w", n, ErrNegativo)
	}
	r := 0
	for (r+1)*(r+1) <= n {
		r++
	}
	return r, nil
}

func protegido(f func()) (err error) {
	defer func() {
		if recuperado := recover(); recuperado != nil {
			err = fmt.Errorf("panic capturado na borda: %v", recuperado)
		}
	}()
	f()
	return nil
}

func main() {
	if _, err := raizAproximada(-4); err != nil {
		fmt.Println("erro tratado:", err)
		fmt.Println("e ErrNegativo?", errors.Is(err, ErrNegativo))
	}
	if r, err := raizAproximada(17); err == nil {
		fmt.Println("raiz aproximada de 17:", r)
	}

	fmt.Println("inicio")
	defer fmt.Println("defer 1 (registrado primeiro)")
	defer fmt.Println("defer 2")
	defer fmt.Println("defer 3 (registrado por ultimo, roda primeiro)")
	fmt.Println("meio")

	err := protegido(func() {
		panic("falha simulada")
	})
	fmt.Println("fronteira devolveu:", err)
	if !errors.Is(ErrNegativo, ErrNegativo) {
		panic("errors.Is quebrado")
	}
	fmt.Println("testes ok")
}
```

3. A saida mostra a ordem: `inicio`, `meio` e depois os defers em ordem **inversa** de registro (3, 2, 1) — pilha LIFO, a mesma intuicao do destrutor em escopo das outras trilhas.

4. `recover` dentro da funcao adiada transforma o panic em erro **na borda** (uma requisicao, um item de uma fila): o programa sobrevive, mas o panic continua sendo um defeito registrado no erro, nao um caminho normal. Capturar para ignorar seria esconder o bug.

5. Classificacao modelo: entrada do usuario fora do formato → `error`; arquivo que nao abriu → `error` com contexto; indice fora do slice por bug interno → `panic` (ou checagem que o torna erro na fronteira); invariante "impossivel" quebrada → `panic`; timeout de rede → `error`. A pergunta divisora e: quem pode **fazer algo** com isso? Se um chamador razoavel trata, e erro; se ninguem trata e o programa esta errado, e panic.

**Autoverificacao:** `errors.Is` enxergando atraves do `%w`, defers em ordem inversa e o panic virando erro so na funcao de fronteira.
