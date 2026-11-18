---
layout: default
---

# Aula 06 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../06.md#13-exercicios-obrigatorios).

```go
package main

import "fmt"

type Conta struct {
	Titular string
	saldo   float64
}

func (c Conta) SaldoAtual() float64 {
	return c.saldo
}

func (c *Conta) Depositar(valor float64) {
	if valor > 0 {
		c.saldo += valor
	}
}

type Falante interface {
	Falar() string
}

type Pessoa struct {
	Nome string
}

func (p Pessoa) Falar() string {
	return "Ola, eu sou " + p.Nome
}

type Robo struct {
	Modelo string
}

func (r Robo) Falar() string {
	return "BIP. Unidade " + r.Modelo
}

type Endereco struct {
	Rua    string
	Cidade string
}

type PessoaCompleta struct {
	Nome     string
	Endereco Endereco
}

type Logger struct {
	Prefixo string
}

func (l Logger) Log(mensagem string) {
	fmt.Printf("[%s] %s\n", l.Prefixo, mensagem)
}

type Servico struct {
	Logger
	Nome string
}

func main() {
	conta := Conta{Titular: "Ana"}
	conta.Depositar(150.75)
	fmt.Printf("saldo de %s: %.2f\n", conta.Titular, conta.SaldoAtual())

	falantes := []Falante{Pessoa{Nome: "Beto"}, Robo{Modelo: "X-9"}}
	for _, f := range falantes {
		fmt.Println(f.Falar())
	}

	completa := PessoaCompleta{
		Nome:     "Cida",
		Endereco: Endereco{Rua: "Rua A, 123", Cidade: "Recife"},
	}
	fmt.Printf("%s mora em %s, %s\n", completa.Nome, completa.Endereco.Rua, completa.Endereco.Cidade)

	servico := Servico{Logger: Logger{Prefixo: "api"}, Nome: "principal"}
	servico.Log("metodo promovido pelo embedding")
	if conta.SaldoAtual() != 150.75 {
		panic("saldo inesperado")
	}
	fmt.Println("testes ok")
}
```

2. `SaldoAtual` le, entao receiver por valor basta e copia centavos. `Depositar` com receiver por valor compilaria igual e **nao mudaria nada** na conta original — o teste do saldo e o que denuncia. Regra pratica: metodo que muta usa ponteiro; a consistencia (todos no mesmo estilo quando ha mistura) vem depois.

3. `Falante` e satisfeita implicitamente: `Pessoa` e `Robo` nem sabem que a interface existe. E o desenho que deixa a interface ser definida por quem **usa** (o laco), nao por quem implementa.

**Autoverificacao:** saldo 150.75 apos o deposito, os dois `Falar()` diferentes pela mesma interface e o `Log` chamado direto no `Servico`.
