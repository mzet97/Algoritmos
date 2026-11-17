# Aula 08a - Raciocínio discreto e algoritmos em Go

---

## Objetivo da Aula

Escrever a pré-condição e a pós-condição de uma função, declarar o invariante de um laço, argumentar terminação por uma grandeza que diminui e provar um caso recursivo por indução — tudo em Go, com a biblioteca padrão e `go vet` limpo. Pré-requisitos: aulas 01 a 08. Não precisa saber ponteiros internos, concorrência, `cgo` nem benchmarks formais.

Esta aula é o módulo G2 do mapa: quem domina invariantes aqui carrega o raciocínio para concorrência (10–11), testes e profiling (14, 17) e protocolos (16 em diante), onde a invariante violada não dá erro gentil — dá corrida, vazamento de goroutine ou cliente travado.

---

## 1. Contrato antes do código

Tome a busca binária sobre uma fatia ordenada: devolver o índice do valor e dizer se ele existe.

- **Pré-condição:** a fatia está ordenada em ordem não decrescente.
- **Pós-condição:** se o valor existe, o índice devolvido aponta para ele; se não existe, o segundo retorno é `false`.
- **Caso especial:** fatia vazia devolve `(0, false)` sem comparar nada. Repetidos admitem qualquer índice válido; documente em vez de fingir unicidade.

Em Go, o múltiplo retorno `(int, bool)` torna a ausência parte do contrato verificada pelo compilador: quem ignora o `bool` recebe um aviso do `go vet` em alguns padrões e, principalmente, um código revisável. O tipo não prova que o índice está correto — essa parte continua sendo um argumento sobre o laço.

Registre hipóteses com testes de tabela (o formato idiomático de Go), não com comentários esperançosos:

```go
casos := []struct {
    nome    string
    valores []int
    alvo    int
    querIdx int
    querOk  bool
}{
    {"vazia", nil, 5, 0, false},
    {"unitario acha", []int{5}, 5, 0, true},
    {"unitario falta", []int{5}, 4, 0, false},
}
```

Cada linha é uma entrada do contrato com saída esperada. Quando um caso falha, o nome diz qual.

## 2. O invariante que sobrevivem às iterações

Mantenha `ini` e `fim` com a promessa:

```text
se o valor está na fatia, então ele está em valores[ini:fim]
```

No início, a fatia é inteira e a promessa vale. A cada passo, compare com o meio: igualdade entrega a pós-condição; `alvo < meio` descarta a direita (`fim = meio`); maior descarta a esquerda (`ini = meio + 1`). Quando `ini == fim`, a fatia está vazia: devolva `(0, false)`.

**Terminação:** `fim - ini` é um `int` não negativo que diminui estritamente a cada iteração sem acerto. Não existe sequência infinita assim.

Duas diferenças em relação a C merecem atenção explícita:

1. **Índice fora da faixa entra em pânico, não corrompe.** `valores[meio]` com índice inválido aborta com `index out of range`. O invariante `ini <= meio < fim` é o que prova que isso nunca acontece — a prova continua necessária mesmo com a rede de segurança.
2. **Fatia é referência, não cópia.** Passar `valores []int` não copia os elementos; se outro código mutar a fatia durante a busca (outra goroutine, um `append` que realoca), a pré-condição "ordenada" pode ruir no meio. Em código sequencial com dono único, isso não acontece; em código concorrente, é exatamente o tipo de invariante que mutexes e canais protegem nas aulas 10–11.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/08a_busca.go) contém exatamente o programa abaixo. Rode com a ferramenta `go`, sem módulo extra:

```bash
go vet teoria/go/exemplos/08a_busca.go && go run teoria/go/exemplos/08a_busca.go
```

```go
package main

import (
	"fmt"
	"math/rand"
	"sort"
	"time"
)

// BuscaBinaria devolve (indice, true) se alvo esta em valores, ou (0, false).
//
// Pre-condicao: valores ordenada em ordem nao decrescente.
// Invariante: se alvo esta na fatia, esta em valores[ini:fim].
func BuscaBinaria(valores []int, alvo int) (int, bool) {
	ini, fim := 0, len(valores)
	for ini < fim {
		meio := ini + (fim-ini)/2
		switch {
		case valores[meio] == alvo:
			return meio, true
		case alvo < valores[meio]:
			fim = meio
		default:
			ini = meio + 1
		}
	}
	return 0, false
}

// BuscaLinear e a referencia simples: percorre tudo, sem exigir ordenacao.
func BuscaLinear(valores []int, alvo int) (int, bool) {
	for i, v := range valores {
		if v == alvo {
			return i, true
		}
	}
	return 0, false
}

// checarPropriedade confere o CONTRATO (nao a igualdade com a referencia):
// com repetidos, dois indices diferentes podem ser ambos corretos.
func checarPropriedade(tentativas int) {
	rng := rand.New(rand.NewSource(42)) // semente fixa: a falha e reproduzivel
	for t := 0; t < tentativas; t++ {
		n := rng.Intn(51)
		valores := make([]int, n)
		for i := range valores {
			valores[i] = rng.Intn(201) - 100
		}
		sort.Ints(valores)
		alvo := rng.Intn(221) - 110
		idx, ok := BuscaBinaria(valores, alvo)
		tem := sort.SearchInts(valores, alvo) < len(valores) && valores[sort.SearchInts(valores, alvo)] == alvo
		if tem {
			if !ok || valores[idx] != alvo {
				panic(fmt.Sprintf("tentativa %d: indice invalido", t))
			}
		} else if ok {
			panic(fmt.Sprintf("tentativa %d: falso positivo", t))
		}
	}
}

func medir(repeticoes int, f func()) float64 {
	inicio := time.Now()
	for i := 0; i < repeticoes; i++ {
		f()
	}
	return time.Since(inicio).Seconds() / float64(repeticoes)
}

func main() {
	casos := []struct {
		nome    string
		valores []int
		alvo    int
		querOk  bool
	}{
		{"vazia", nil, 5, false},
		{"unitario acha", []int{5}, 5, true},
		{"unitario falta", []int{5}, 4, false},
		{"inicio", []int{1, 3, 5, 7}, 1, true},
		{"fim", []int{1, 3, 5, 7}, 7, true},
		{"ausente", []int{1, 3, 5, 7}, 4, false},
		{"repetidos", []int{2, 2, 2}, 2, true},
	}
	for _, c := range casos {
		idx, ok := BuscaBinaria(c.valores, c.alvo)
		if ok != c.querOk || (ok && c.valores[idx] != c.alvo) {
			panic("caso " + c.nome + ": contrato violado")
		}
	}

	checarPropriedade(2000)

	grande := make([]int, 1_000_000)
	for i := range grande {
		grande[i] = i * 2
	}
	for _, alvo := range []int{1_999_998, 1_999_999} {
		if _, ok := BuscaBinaria(grande, alvo); ok != (alvo == 1_999_998) {
			panic("grande: contrato violado")
		}
		if _, ok := BuscaLinear(grande, alvo); ok != (alvo == 1_999_998) {
			panic("grande: referencia violada")
		}
	}

	tLin := medir(5, func() { BuscaLinear(grande, 1_999_999) })
	tBin := medir(200, func() { BuscaBinaria(grande, 1_999_999) })
	fmt.Printf("linear: %.6fs/media (5x)\n", tLin)
	fmt.Printf("binaria: %.9fs/media (200x)\n", tBin)
}
```

Saída esperada (tempos variam por máquina; a razão importa, não o dígito):

```text
linear: ~0.00Xs/media (5x)
binaria: ~0.0000000Xs/media (200x)
```

O oráculo aqui é `sort.SearchInts` da biblioteca padrão, exercitado sobre os mesmos casos — mas note o detalhe: usamos o oráculo para decidir *presença*, e o contrato (`valores[idx] == alvo`) para validar o índice. Com repetidos, `SearchInts` devolve *um* índice válido, não necessariamente o mesmo da nossa busca; exigir igualdade seria transformar resposta válida em falha falsa.

### O que a memória faz aqui?

`grande` aloca um milhão de `int` de uma vez: custo `O(n)` explícito. As buscas usam `O(1)` espaço adicional e não alocam (`go vet` mais o relatório de escape da aula 09 confirmam o padrão). Medir antes de verificar a correção inverte a ordem: primeiro os casos e a propriedade, depois o cronômetro.

## 4. Indução: recursão com argumento, não com esperança

```go
func somaAte(n uint64) uint64 {
	if n == 0 {
		return 0
	}
	return n + somaAte(n-1)
}
```

Base: `somaAte(0) == 0`. Passo: supondo `somaAte(n-1)` correto, somar `n` entrega `1..n`. Na máquina, cada chamada pendente consome stack de goroutine: a pilha cresce sob demanda até o limite (1 GB em 64 bits por padrão), e estourá-lo aborta o programa — sem `recover` que salve. Prova de correção não remove limite de máquina: declare a profundidade máxima no contrato ou use laço.

## 5. Conjuntos, relações e o custo do `map`

Relações aparecem sempre que um dado aponta para outro. Um grafo é um conjunto de vértices mais pares (arestas). A BFS encontra o caminho mais curto em arestas, com o invariante: a fila só contém vértices alcançáveis, e cada vértice entra em `visitados` antes de entrar na fila — logo sai no máximo uma vez e o laço termina mesmo sem alcançar o destino.

```go
func bfsCaminho(grafo map[string][]string, origem, destino string) []string {
	visitados := map[string]struct{}{origem: {}}
	type item struct {
		no      string
		caminho []string
	}
	fila := []item{{origem, []string{origem}}}
	for len(fila) > 0 {
		atual := fila[0]
		fila = fila[1:]
		if atual.no == destino {
			return atual.caminho
		}
		for _, viz := range grafo[atual.no] {
			if _, ok := visitados[viz]; !ok {
				visitados[viz] = struct{}{}
				fila = append(fila, item{viz, append(append([]string{}, atual.caminho...), viz)})
			}
		}
	}
	return nil
}
```

O conjunto é `map[string]struct{}`: pertinência `O(1)` em média pelo hash. O alerta honesto: médio não é pior caso; entrada adversária degrada, e iteração de `map` tem ordem aleatória — nunca dependa dela. E atenção ao `append` compartilhando o array do caminho: o código acima copia antes de estender, porque fatiar não copia.

## 6. Erros comuns e implicações defensivas

1. **Buscar sem garantir ordenação:** a binária devolve resposta errada em silêncio. Valide ou documente a pré-condição.
2. **Ignorar o segundo retorno:** descartar o `bool` com `_` transfere a checagem para lugar nenhum. Trate ausência no ponto da chamada.
3. **Recursão sem limite declarado:** estouro de pilha aborta o programa. Prefira laço ou limite a profundidade no contrato.
4. **Medir antes de provar:** benchmark de algoritmo errado mede a velocidade do erro.
5. **Semente solta:** teste de propriedade sem `NewSource(42)` gera falha irreproduzível.
6. **Ordem de `map`:** iteração aleatória não é bug, é especificação. Código que depende dela quebra sem aviso.

## 8. Exercícios obrigatórios

1. Reescreva a prova do invariante da busca binária (início, preservação, término) sem consultar a seção 2.
2. Implemente `bfsCaminho` para um grafo com ciclo e prove que ela termina mesmo com destino inalcançável. Conte vértices visitados.
3. Troque a semente para `7` e `1234`; o resultado muda? Explique o que `rand.NewSource(time.Now().UnixNano())` quebraria.
4. Escreva a soma `1..n` iterativa com invariante declarado e mostre um `n` em que a versão recursiva aborta na sua máquina.

Use [pistas graduais](exercicios/08a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/08a_solucoes.md) depois de registrar sua própria explicação.

## 9. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- declarar pré-condição, pós-condição e convenção de repetidos e de fatia vazia;
- explicar por que o invariante vale antes e depois de cada iteração;
- indicar a grandeza que diminui e provar o término;
- rodar `go vet` limpo, executar o exemplo e confrontar `O(log n)` com a medição;
- explicar por que o teste de propriedade confere o contrato e não a igualdade com a referência;
- apontar o que o pânico de índice garante e o que ele não garante.

**Próxima aula:** ponteiros, memória e escape analysis. O raciocínio por invariantes desta aula sustenta quem pode tocar cada byte: dono único, empréstimo e fuga para o heap.

## 7. Ponte com Python e JavaScript

Python tem `int` arbitrário e `in` legível; JavaScript usa `Number` de 64 bits e `Array.prototype.sort` sem comparador ordena como texto. Go usa `int` com largura de plataforma e `sort.Ints` numérico. A identidade do invariante é a mesma nas três linguagens; o que muda é a rede de segurança (pânico indexado em Go e Python, `undefined` silencioso em JS) e o preço do erro.
