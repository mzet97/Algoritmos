---
layout: default
---

# Aula 09 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../09.md#13-exercicios-obrigatorios).

```go
package main

import "fmt"

func dobrar(p *int) {
	*p = *p * 2
}

type Contador struct {
	valor int
}

func (c *Contador) Incrementar() {
	c.valor++
}

func (c Contador) Valor() int {
	return c.valor
}

type Retangulo struct {
	Largura, Altura float64
}

func areaPorValor(r Retangulo) float64 {
	return r.Largura * r.Altura
}

func escalarPorPonteiro(r *Retangulo, fator float64) {
	r.Largura *= fator
	r.Altura *= fator
}

func main() {
	n := 21
	dobrar(&n)
	fmt.Println("dobrado:", n)

	contador := Contador{}
	contador.Incrementar()
	contador.Incrementar()
	fmt.Println("contador:", contador.Valor())

	r := Retangulo{Largura: 3, Altura: 4}
	fmt.Println("area:", areaPorValor(r))
	escalarPorPonteiro(&r, 2)
	fmt.Printf("escalado: %.0fx%.0f area=%.0f\n", r.Largura, r.Altura, areaPorValor(r))

	p := new(int)
	fmt.Println("new(int) nasce:", *p)
	*p = 7
	fmt.Println("new(int) depois:", *p)
	if n != 42 || contador.Valor() != 2 {
		panic("resultados inesperados")
	}
	fmt.Println("testes ok")
}
```

3. `areaPorValor` recebe uma copia do retangulo: pode ler tudo, nao muda nada fora. `escalarPorPonteiro` recebe o endereco e muda o retangulo do chamador — a mesma conversa de referencia e valor da [aula 02](02-solucoes.md), agora com structs. Copiar struct pequena e barata e segura; struct grande ou identidade compartilhada pede ponteiro, declarado no tipo da funcao, nao descoberto no bug.

5. Roteiro do escape analysis (executado nesta rodada num projeto scratch): `go build -gcflags="-m" .` imprime decisoes como `moved to heap:` e `escapes to heap` para variaveis cujo endereco sobrevive a funcao. O ponto nao e decorar mensagens: e ligar a **causa** (retornar o ponteiro, guarda-lo num slice global, fecha-lo numa goroutine) ao efeito (heap em vez de stack). Sem causa nomeada, a leitura do relatorio vira astrologia.

**Autoverificacao:** 42 apos `dobrar`, contador em 2 e uma linha de escape do seu projeto explicada com a causa.
