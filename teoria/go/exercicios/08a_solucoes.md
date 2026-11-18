---
layout: default
---

# Soluções comentadas da aula 08a

## 1. Prova do invariante

- **Início:** `ini = 0`, `fim = len(valores)`; a fatia é inteira e a promessa vale.
- **Preservação:** com `ini < fim`, `meio` está dentro da fatia (indexação verificada nunca dispara se o invariante vale). Igualdade entrega a pós-condição; os ramos descartam metades que a ordenação prova estarem do lado errado.
- **Término:** `fim - ini` diminui estritamente; ao sair, `ini == fim` e `(0, false)` é correto.

## 2. BFS com ciclo e destino inalcançável

```go
func bfsContando(grafo map[string][]string, origem string) int {
	visitados := map[string]struct{}{origem: {}}
	fila := []string{origem}
	removidos := 0
	for len(fila) > 0 {
		atual := fila[0]
		fila = fila[1:]
		removidos++
		for _, v := range grafo[atual] {
			if _, ok := visitados[v]; !ok {
				visitados[v] = struct{}{}
				fila = append(fila, v)
			}
		}
	}
	return removidos
}
```

Cada vértice entra em `visitados` uma vez; `removidos <= |V|` e o laço termina com ou sem destino.

## 3. Sementes

As três sementes devem passar com a mesma conclusão. A semente fixa torna a sequência de 2000 casos reproduzível: uma falha aponta a tentativa exata (`t`). Com tempo real como semente, cada execução testa casos diferentes e uma falha pode não se repetir — inútil como regressão.

## 4. Soma iterativa e aborto da recursão

```go
func somaAte(n uint64) uint64 {
	var acumulado uint64 // invariante: soma de 1..(i-1)
	for i := uint64(1); i <= n; i++ {
		acumulado += i // uint64 embrulha em silencio: declare o n maximo
	}
	return acumulado
}
```

Com `n` da ordem de centenas de milhões, a versão recursiva aborta por estouro de pilha da goroutine (sem `recover` que salve). A iterativa usa `O(1)` espaço. Note ainda: `uint64` em Go **embrulha** em vez de entrar em pânico — diferente de Rust em debug. O contrato precisa declarar o `n` máximo, não basta torcer.
