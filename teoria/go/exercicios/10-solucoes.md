---
layout: default
---

# Aula 10 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../10.md#13-exercicios-obrigatorios).

```go
package main

import (
	"fmt"
	"time"
)

func main() {
	// Canal sem buffer: encontro entre goroutine e main.
	semBuffer := make(chan int)
	go func() {
		semBuffer <- 42
	}()
	fmt.Println("sem buffer:", <-semBuffer)

	// Canal com buffer: o envio nao espera o receptor.
	comBuffer := make(chan string, 1)
	comBuffer <- "depositado antes da leitura"
	fmt.Println("com buffer:", <-comBuffer)

	// select: o primeiro canal pronto vence.
	rapido := make(chan string)
	lento := make(chan string)
	go func() {
		time.Sleep(10 * time.Millisecond)
		rapido <- "rapido"
	}()
	go func() {
		time.Sleep(200 * time.Millisecond)
		lento <- "lento"
	}()
	select {
	case msg := <-rapido:
		fmt.Println("select escolheu:", msg)
	case msg := <-lento:
		fmt.Println("select escolheu:", msg)
	}

	// Timeout: ou o trabalho termina, ou o relogio vence.
	trabalho := make(chan string)
	go func() {
		time.Sleep(200 * time.Millisecond)
		trabalho <- "terminou"
	}()
	select {
	case msg := <-trabalho:
		fmt.Println("trabalho:", msg)
	case <-time.After(50 * time.Millisecond):
		fmt.Println("timeout: desisti de esperar")
	}
	fmt.Println("testes ok")
}
```

3. Se os dois canais estiverem prontos quando o `select` for avaliado, a escolha e pseudoaleatoria por desenho da linguagem — nao escreva logica que dependa de prioridade entre casos prontos.

5. Channel vence variavel global compartilhada quando um lado precisa **esperar** o dado: o receptor dorme ate a entrega, sem girar consumindo CPU e sem mutex+condicao escritos a mao. Compartilhar memoria continua certo para estado que muitos leem e poucos mudam sob trava curta (a [aula 11](11-solucoes.md) mede os dois); a frase do Go e sobre o padrao comum, nao uma proibicao.

**Autoverificacao:** o envio com buffer completa antes da leitura, o timeout vence o trabalho de 200 ms e nenhum `select` seu depende de prioridade entre prontos.
