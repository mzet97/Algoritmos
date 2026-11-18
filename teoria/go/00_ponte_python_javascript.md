---
layout: default
---

# Aula 00 — Ponte de partida: o que Python e JavaScript te ensinaram (e o que muda aqui)

## Objetivos verificáveis

Ao final, você deverá: mapear o que Python e JavaScript ensinaram (tipagem dinâmica, GC, closures) para o que muda em Go (tipagem estática, compilação, goroutines), e prever quais hábitos dinâmicos geram erros em Go.
Mediremos com níveis de proficiência:
- **Nível 1 — Recordar:** `definir tipagem estática vs. dinâmica e compilação vs. interpretação`;
- **Nível 2 — Aplicar:** `prever o erro de um hábito dinâmico transplantado para Go e corrigir`;
- **Nível 3 — Projetar:** `projetar seu plano de transição justificando o que reaproveitar e o que desaprender`.

**Competências SDD:** FND. **Pré-requisitos:** saber variáveis, condicionais, laços e funções em Python ou JavaScript. Nada de compiladores, tipos estáticos ou terminais é pressuposto sem explicação.

**Objetivo observável:** ao terminar, você deverá dizer, para cada hábito trazido de Python/JS, o que continua valendo em Go e o que quebra — e compilar um programa que demonstra as três quebras clássicas sem nenhuma falhar.

**Motivação:** todo leitor deste livro chega com reflexos de outra linguagem: somar `"1" + 1`, comparar com `==` sem pensar, partir do `None`/`undefined` como "vazio universal". Go não pune esses reflexos em produção — ele os recusa na compilação. Esta aula traduz os reflexos antes que o compilador precise gritar.

## 1. Contrato antes do código

- **Execução:** Python/JS executam o texto; Go compila antes de executar. `go run` compila para um binário temporário e roda; `go build` deixa o binário no disco. Erro de tipo não é exceção em produção — é recusa na sua máquina.
- **Zero é o vazio:** toda variável nasce com valor zero do tipo (`0`, `""`, `false`, `nil` para referências). Não existe `undefined`: ler antes de atribuir dá zero, não erro — e é por isso que o zero precisa ser um valor válido do seu desenho.
- **Sem coerção implícita:** `"1" + 1` não compila. Conversão é chamada explícita (`strconv.Atoi`), e a chamada devolve `(valor, erro)` — o erro é valor, não exceção.
- **Cópia rasa por padrão:** `b := a` numa slice copia o cabeçalho (ponteiro, tamanho, capacidade), não os elementos — como apelidar a mesma lista em Python. Copiar elementos exige `copy` explícito.
- **Mapa é referência:** mapas se comportam como objetos JS passados por referência: quem recebe, muta o original. Não há cópia escondida.
- **Fora do contrato:** toolchain multiplataforma detalhada por SO (Windows/macOS/Linux diferem em caminho e shell; `filepath` trata, o shell não) e módulos remotos — `go.mod` local basta para este volume.

## 2. Onde o reflexo quebra (e onde continua)

| Reflexo de Python/JS | Em Go |
|---|---|
| Somar string com número e "ver no que dá" | Não compila; conversão explícita com erro |
| `None`/`undefined` como vazio universal | Cada tipo tem seu zero; `nil` só para ponteiros, slices, mapas, canais, interfaces |
| `try/except` ao redor de tudo | `if err != nil` no ponto da chamada; quem ignora erro escreve `_` e assume |
| Apelidar lista e esperar independência | Slice apelidada compartilha o array; `copy` é explícito |
| `dict` copiado por atribuição | Mapa atribuído compartilha; cópia é laço |

O que continua: laço, condicional, função e o hábito de testar. O que muda é onde o erro aparece: em Python ele aparece na execução do cliente; em Go, na sua compilação — mais cedo, mais barato, mais barulhento.

## 3. Exemplo completo e executável

```go
package main

import (
	"fmt"
	"strconv"
)

func dividir(a, b int) (int, error) {
	if b == 0 {
		return 0, fmt.Errorf("divisao por zero")
	}
	return a / b, nil
}

func main() {
	// Zero de cada tipo: declarar e ler, sem atribuir.
	var inteiro int
	var texto string
	var condicao bool
	fmt.Println("zeros:", inteiro, texto == "", condicao)

	// Sem coercao: a conversao e explicita e devolve erro.
	numero, err := strconv.Atoi("42")
	if err != nil {
		panic(err)
	}
	_, errInvalido := strconv.Atoi("quarenta")
	if errInvalido == nil {
		panic("texto deveria falhar")
	}
	fmt.Println("conversao explicita:", numero, "| texto invalido recusado:", errInvalido != nil)

	// Slice: atribuir apelida; copy independe.
	original := []int{1, 2, 3}
	apelido := original
	apelido[0] = 99
	independente := make([]int, len(original))
	copy(independente, original)
	independente[0] = 1
	fmt.Println("apelido compartilha:", original[0] == 99,
		"| copia independe:", independente[0] == 1 && original[0] == 99)

	// Erro como valor: quem chama decide, na hora.
	if q, err := dividir(10, 2); err != nil {
		panic(err)
	} else {
		fmt.Println("10/2 =", q)
	}
	if _, err := dividir(10, 0); err == nil {
		panic("divisao por zero deveria falhar")
	}
	fmt.Println("testes ok")
}
```

Execução (`go vet` + `go run -race`):

```bash
go vet ./... && go run -race 00_ponte.go
```

Saída esperada (determinística):

```text
zeros: 0 true false
conversao explicita: 42 | texto invalido recusado: true
apelido compartilha: true | copia independe: true
10/2 = 5
testes ok
```

O programa que somaria `"1" + 1` não está aqui de propósito: ele não compila, e exemplo que não compila isolado se rotula **trecho conceitual** — esta aula o declara em texto em vez de fingi-lo em código.

### O que a memória faz aqui?

`apelido := original` copia 24 bytes de cabeçalho (ponteiro + len + cap), não os elementos: duas fatias, um array. `copy` percorre e duplica elemento a elemento — O(n) pago uma vez, em vez de aliasing pago para sempre. Mapa (não mostrado no exemplo, coberto na [aula 04](04.md)) segue a mesma regra do apelido: atribuição compartilha.

## 4. Erros comuns e implicações defensivas

1. **Esperar `undefined`:** ler variável não atribuída dá zero e segue — bug silencioso quando zero é inválido no seu domínio. Valide na fronteira.
2. **Converter sem olhar o erro:** `n, _ := strconv.Atoi(s)` transforma entrada hostil em zero silencioso. O `_` é confissão, não atalho.
3. **Partir do `nil` como vazio iterável:** `for` sobre slice `nil` funciona (zero iterações); escrever em mapa `nil` dá panic. Zero nem sempre é usável.
4. **Confundir `go run` com interpretador:** cada `go run` recompila; medir desempenho nele mede compilação junto. Benchmark usa binário (`go test -bench`, [aula 14](exercicios/14-solucoes.md)).
5. **Tratar erro como exceção:** não há `try` para salvar depois. Erro ignorado hoje é incidente sem rastro amanhã — o `if err != nil` é o log mínimo da linguagem.

## 5. Ponte com as demais trilhas

C recusa menos na compilação e cobra mais no sanitizer; Rust recusa mais (ownership) e cobra com invariantes; Python adia quase tudo para a execução — por isso valida na fronteira. Em JavaScript, `undefined` é um valor que viaja; em Go, a viagem é recusada na origem. A pergunta é sempre a mesma: *quando* o erro aparece — e quem paga por ele.

---

### Como esta aula será avaliada

| Critério | Peso |
| -------- | ---: |
| Conceitos e vocabulário (nível 1) | 20% |
| Previsão de comportamento e diagnóstico de erros (nível 2) | 40% |
| Projeto e justificativa de decisões (nível 3) | 30% |
| Registro de comandos, saídas e interpretação | 10% |

**Aprovação:** nota mínima 7,0; pelo menos 60% dos pontos devem vir de previsão e diagnóstico (níveis 2 e 3), não apenas de código que executa.

**Para se aprofundar:**

1. Alan Donovan e Brian Kernighan, *The Go Programming Language* (Addison-Wesley, ISBN 978-0134190440); 2. Documentação oficial em [go.dev/doc](https://go.dev/doc/); 3. Referência de pacotes em [pkg.go.dev](https://pkg.go.dev/).

## 6. Checklist do que esta aula garante

- [ ] Explico `go run` vs `go build` e onde o erro de tipo aparece em cada linguagem.
- [ ] Demonstro o zero de cada tipo e digo quando ele é armadilha.
- [ ] Converto texto em número tratando o erro, e digo o que o `_` esconderia.
- [ ] Distingo apelido de cópia em slice, com o custo de cada um.
- [ ] Trato erro como valor no ponto da chamada, sem `try` para depois.

**Referências primárias:** [Especificação de Go](https://go.dev/ref/spec) para valor zero e conversões; [Tour of Go](https://go.dev/tour/welcome/1) como prática guiada. Nenhuma comparação com Python/JS nesta aula é juízo de valor — é mapa de reflexos.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
