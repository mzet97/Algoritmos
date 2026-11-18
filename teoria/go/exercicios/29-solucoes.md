---
layout: default
---

# Aula 29 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../29.md#23-exercicios-obrigatorios). A soma reescrita em Go puro e a API idiomatica sobre uma "biblioteca" (aqui, `compress/gzip` da padrao no papel da nativa):

```go
package main

import (
	"bytes"
	"compress/gzip"
	"errors"
	"fmt"
	"io"
)

func somar(valores []int64) int64 {
	var total int64
	for _, v := range valores {
		total += v
	}
	return total
}

// Handle opaco: o campo nao exportado esconde a representacao (exercicio 2).
type compressor struct {
	buf *bytes.Buffer
	w   *gzip.Writer
}

var errFechado = errors.New("compressor ja fechado")

func novoCompressor() *compressor {
	var buf bytes.Buffer
	return &compressor{buf: &buf, w: gzip.NewWriter(&buf)}
}

func (c *compressor) Escrever(dados []byte) error {
	if c.w == nil {
		return errFechado
	}
	_, err := c.w.Write(dados)
	return err
}

func (c *compressor) Fechar() ([]byte, error) {
	if c.w == nil {
		return nil, errFechado
	}
	if err := c.w.Close(); err != nil {
		return nil, err
	}
	c.w = nil
	return c.buf.Bytes(), nil
}

func main() {
	valores := make([]int64, 100000)
	for i := range valores {
		valores[i] = int64(i)
	}
	fmt.Println("soma Go puro confere:", somar(valores) == int64(len(valores)-1)*int64(len(valores))/2)

	c := novoCompressor()
	original := []byte("dados repetidos dados repetidos dados repetidos")
	if err := c.Escrever(original); err != nil {
		panic(err)
	}
	comprimido, err := c.Fechar()
	if err != nil {
		panic(err)
	}
	leitor, err := gzip.NewReader(bytes.NewReader(comprimido))
	if err != nil {
		panic(err)
	}
	volta, _ := io.ReadAll(leitor)
	fmt.Println("ida e volta:", bytes.Equal(volta, original), "| segundo Fechar e erro:", func() bool {
		_, err := c.Fechar()
		return errors.Is(err, errFechado)
	}())
	fmt.Println("testes ok")
}
```

1. A soma em Go e isto — e e dificil a versao em C vencer: o laco vetoriza, nao ha conversao de dados e nao ha pedagio de chamada. A versao C faria sentido conceitualmente se a soma morasse numa biblioteca C existente e enorme, ou exigisse uma instrucao que o Go nao expoe — e mesmo assim, em **lote** (crossing grosso), nunca elemento a elemento.

3. Tres riscos de callbacks C chamando Go: (a) a thread e do C — ela nao conhece o runtime Go, e bloquear nela pode travar quem o C esperava liberar; (b) ponteiros Go guardados pelo C para "chamar depois" violam as regras de ponteiro do cgo (o GC nao foi consultado); (c) um `panic` Go atravessando a fronteira C e comportamento indefinido — callback Go para C se escreve sem panic, devolvendo codigo.

4. Crossing fino (uma chamada por elemento) paga por chamada: salvamento de registradores, transicao de stack, checagens do cgo. Crossing grosso (um lote por chamada) paga uma vez e processa milhoes — a razao entre trabalho util e pedagio e a metrica da fronteira, como na [aula 18](18-solucoes.md).

5. A API idiomatica e a do `compressor` acima: construtor que devolve o handle, metodos que devolvem `error` (sentinela tipada para o estado fechado), `Fechar` com dono unico. O codigo inteiro que a biblioteca C devolveria vira `error` **na fronteira**; dentro do Go, ninguem compara inteiro.

6. Deploy com `.so`/`.dll`/`.dylib`: um artefato por par sistema-arquitetura, a biblioteca precisa existir (ou ser embarcada) em cada destino, e a versao dela vira dependencia operacional — o binario unico do Go puro vira uma matriz, como na aula 18.

**Autoverificacao:** soma confere pela formula, ida e volta do compressor iguais e o segundo `Fechar` e erro tipado.
