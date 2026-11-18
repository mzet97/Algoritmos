---
layout: default
---

# Aula 30 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../30.md#22-exercicios-obrigatorios). Nucleo do balanceador exercitado contra backends `httptest`:

```go
package main

import (
	"fmt"
	"io"
	"log/slog"
	"net/http"
	"net/http/httptest"
	"strings"
	"sync"
	"time"
)

type backend struct {
	nome          string
	url           string
	peso          int
	saudavel      bool
	falhasSeguidas int
	mu            sync.Mutex
}

type balanceador struct {
	mu       sync.Mutex
	backends []*backend
	atual    map[*backend]int
	servidas map[string]int
}

func novoBalanceador(backends []*backend) *balanceador {
	return &balanceador{backends: backends, atual: map[*backend]int{}, servidas: map[string]int{}}
}

// Weighted round-robin suave: a cada escolha, soma o peso em todos os
// saudaveis, escolhe o maior placar e subtrai o peso total dele.
func (b *balanceador) escolher() *backend {
	b.mu.Lock()
	defer b.mu.Unlock()
	total := 0
	var melhor *backend
	for _, be := range b.backends {
		be.mu.Lock()
		saudavel := be.saudavel
		be.mu.Unlock()
		if !saudavel {
			continue
		}
		total += be.peso
		b.atual[be] += be.peso
		if melhor == nil || b.atual[be] > b.atual[melhor] {
			melhor = be
		}
	}
	if melhor == nil {
		return nil
	}
	b.atual[melhor] -= total
	b.servidas[melhor.nome]++
	return melhor
}

func (b *balanceador) verificarSaude(cliente *http.Client) {
	for _, be := range b.backends {
		resp, err := cliente.Get(be.url + "/healthz")
		ok := err == nil && resp.StatusCode == http.StatusOK
		if resp != nil {
			io.Copy(io.Discard, resp.Body)
			resp.Body.Close()
		}
		be.mu.Lock()
		if ok {
			be.saudavel = true
			be.falhasSeguidas = 0
		} else {
			be.falhasSeguidas++
			if be.falhasSeguidas >= 2 {
				be.saudavel = false
			}
		}
		be.mu.Unlock()
	}
}

func (b *balanceador) metricasPrometheus() string {
	b.mu.Lock()
	defer b.mu.Unlock()
	var sb strings.Builder
	for nome, n := range b.servidas {
		fmt.Fprintf(&sb, "lb_requisicoes_total{backend=%q} %d\n", nome, n)
	}
	return sb.String()
}

func main() {
	cli := &http.Client{Timeout: time.Second}
	saudavel := httptest.NewServer(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		fmt.Fprint(w, "ok")
	}))
	defer saudavel.Close()
	morto := httptest.NewServer(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		w.WriteHeader(http.StatusInternalServerError)
	}))
	defer morto.Close()

	b := novoBalanceador([]*backend{
		{nome: "forte", url: saudavel.URL, peso: 3, saudavel: true},
		{nome: "fraco", url: saudavel.URL, peso: 1, saudavel: true},
		{nome: "doente", url: morto.URL, peso: 1, saudavel: true},
	})
	b.verificarSaude(cli)
	b.verificarSaude(cli) // segunda falha seguida tira o doente da rotacao
	for i := 0; i < 8; i++ {
		escolhido := b.escolher()
		slog.Info("escolha", "backend", escolhido.nome)
		_ = escolhido
	}
	fmt.Println("servidas:", b.servidas)
	fmt.Print(b.metricasPrometheus())
	fmt.Println("testes ok")
}
```

Leitura esperada das 8 escolhas: `forte` fica com ~6 e `fraco` com ~2 (pesos 3:1 ao longo do ciclo) e `doente` com **zero** — ele saiu da rotacao no health check HTTP (exercicio 2) apos 2 falhas seguidas (exercicio 5), antes de qualquer requisicao de usuario pagar pela doenca dele.

4. O `slog` por escolha registra backend e, num proxy completo, duracao e status — e a linha que falta quando ninguem e escolhido (`melhor == nil`) e log de **erro**, nao de info: todos os backends fora e um fato operacional, nao um detalhe.

**Autoverificacao:** proporcao 3:1 respeitada no ciclo, doente fora com zero servidas e metricas em texto Prometheus.
