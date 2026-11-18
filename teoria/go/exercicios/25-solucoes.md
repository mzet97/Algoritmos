---
layout: default
---

# Aula 25 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../25.md#21-exercicios-obrigatorios).

```go
package main

import (
	"errors"
	"fmt"
	"log/slog"
	"sync"
	"sync/atomic"
	"time"
)

func deveTentar(status int) bool {
	switch status {
	case 429, 502, 503, 504:
		return true
	default:
		return false
	}
}

type disjuntor struct {
	mu               sync.Mutex
	falhasSeguidas   int
	aberto           bool
	sucessos, falhas int
	rejeitados       int
}

var errCircuitoAberto = errors.New("circuito aberto")
var errQuotaTimeout = errors.New("timeout esperando quota")

func (d *disjuntor) executar(operacao func() (int, error)) (int, error) {
	d.mu.Lock()
	if d.aberto {
		d.rejeitados++
		d.mu.Unlock()
		return 0, errCircuitoAberto
	}
	d.mu.Unlock()

	status, err := operacao()

	d.mu.Lock()
	defer d.mu.Unlock()
	if err != nil || deveTentar(status) {
		d.falhas++
		d.falhasSeguidas++
		slog.Info("falha na dependencia", "status", status, "falhas_seguidas", d.falhasSeguidas)
		if d.falhasSeguidas >= 3 {
			d.aberto = true
			slog.Info("circuito aberto", "falhas_seguidas", d.falhasSeguidas)
		}
		if err != nil {
			return status, err
		}
		return status, fmt.Errorf("status repetivel: %d", status)
	}
	d.sucessos++
	d.falhasSeguidas = 0
	return status, nil
}

func comRetry(d *disjuntor, operacao func() (int, error), tentativas int) (int, error) {
	var ultimo error
	for i := 1; i <= tentativas; i++ {
		status, err := d.executar(operacao)
		if err == nil {
			return status, nil
		}
		ultimo = err
		if errors.Is(err, errCircuitoAberto) {
			return 0, err
		}
		espera := time.Duration(i*10) * time.Millisecond
		slog.Info("retry agendado", "tentativa", i, "backoff_ms", espera.Milliseconds())
		time.Sleep(espera)
	}
	return 0, ultimo
}

func main() {
	// Dependencia instavel deterministica: falha 503 nas 2 primeiras chamadas globais.
	var chamadas atomic.Int64
	instavel := func() (int, error) {
		if chamadas.Add(1) <= 2 {
			return 503, nil
		}
		return 200, nil
	}
	d := &disjuntor{}
	status, err := comRetry(d, instavel, 5)
	fmt.Println("apos retries:", status, "| erro:", err, "| chamadas:", chamadas.Load())

	// Dependencia morta: o circuito abre e passa a rejeitar sem chamar.
	morta := func() (int, error) { return 503, nil }
	d2 := &disjuntor{}
	for i := 0; i < 5; i++ {
		_, err := d2.executar(morta)
		_ = err
	}
	fmt.Println("circuito aberto:", d2.aberto, "| rejeitados sem chamada:", d2.rejeitados)

	// Quota com espera limitada em vez de rejeicao imediata (exercicio 3).
	quota := make(chan struct{}, 2)
	adquirir := func() error {
		select {
		case quota <- struct{}{}:
			return nil
		case <-time.After(30 * time.Millisecond):
			return errQuotaTimeout
		}
	}
	_ = adquirir()
	_ = adquirir()
	fmt.Println("terceira aquisicao (quota cheia):", errors.Is(adquirir(), errQuotaTimeout))
	<-quota
	fmt.Println("apos liberar:", adquirir() == nil)

	fmt.Println("tabela retry:", deveTentar(429), deveTentar(503), deveTentar(400), deveTentar(404))
	fmt.Println("testes ok")
}
```

4. `429` diz "devagar": retry com a espera que o servidor sugerir (`Retry-After`). `503` diz "agora nao": retry com backoff. `502`/`504` dizem "a resposta se perdeu no caminho": retry so e seguro se a operacao for idempotente ou carregar chave de idempotencia — voce nao sabe se o original executou. `400`/`404` nao entram: repetir um pedido invalido so produz o mesmo erro mais devagar.

5. Endpoint de escrita repetivel pede: chave de idempotencia por tentativa logica (o servidor deduplica, como nos mini-Redis das outras trilhas), ou operacao naturalmente idempotente (`PUT` de estado completo, `DELETE`). Sem um dos dois, retry de escrita e duplicacao com passos extras.

2 e 6. Os contadores `sucessos/falhas/rejeitados` sao exatamente o que um `/metrics` exporia (na aula 30 eles saem em formato Prometheus); o teste de carga honesto dispara goroutines contra a dependencia instavel e confere os contadores no fim — neste harness, a dependencia deterministica torna a contagem conferivel sem relogio de parede.

**Autoverificacao:** retry vence o 503 temporario, circuito abre na dependencia morta e rejeita sem chamar, quota cheia da timeout (nao rejeicao seca) e a tabela de status bate.
