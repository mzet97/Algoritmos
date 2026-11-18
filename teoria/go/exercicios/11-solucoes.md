---
layout: default
---

# Aula 11 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../11.md#13-exercicios-obrigatorios).

```go
package main

import (
	"fmt"
	"sync"
	"sync/atomic"
)

func main() {
	var wg sync.WaitGroup
	for i := 0; i < 3; i++ {
		wg.Add(1)
		go func(id int) {
			defer wg.Done()
			fmt.Println("goroutine", id, "terminou")
		}(i)
	}
	wg.Wait()
	fmt.Println("todas terminaram")

	var mu sync.Mutex
	contadorMutex := 0
	wg = sync.WaitGroup{}
	for g := 0; g < 10; g++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for i := 0; i < 1000; i++ {
				mu.Lock()
				contadorMutex++
				mu.Unlock()
			}
		}()
	}
	wg.Wait()

	var contadorAtomico atomic.Int64
	wg = sync.WaitGroup{}
	for g := 0; g < 10; g++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for i := 0; i < 1000; i++ {
				contadorAtomico.Add(1)
			}
		}()
	}
	wg.Wait()

	fmt.Printf("mutex=%d atomico=%d\n", contadorMutex, contadorAtomico.Load())
	if contadorMutex != 10000 || contadorAtomico.Load() != 10000 {
		panic("contador divergiu")
	}
	fmt.Println("testes ok")
}
```

4. Channel quando o desenho e **transferir** trabalho ou resultados (produtor/consumidor, pipeline da [aula 13 de Rust](../../rust/exercicios/13-solucoes.md)); mutex quando ha um **estado** que varias goroutines leem e escrevem em operacoes compostas. Contador isolado aceita os tres; o criterio aparece quando o invariante envolve mais de uma variavel.

5. Rode a versao sem trava (retire `mu` do primeiro contador) com `go run -race`: o detector aponta o par de acessos e as goroutines envolvidas. Sem `-race`, o mesmo programa pode imprimir 10000 por sorte de escalonamento — "deu certo" nao e evidencia, como nas aulas de concorrencia de C e C++.

**Autoverificacao:** 10000 exatos nas duas versoes, um relatorio de corrida reproduzido na versao insegura e cada goroutine com `Done` garantido por `defer`.
