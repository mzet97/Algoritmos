---
layout: default
---

# Aula 24 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../24.md#20-exercicios-obrigatorios).

```go
package main

import (
	"fmt"
	"io"
	"log/slog"
	"net/http"
	"net/http/httptest"
	"os"
	"path/filepath"
	"runtime/pprof"
	"strings"
	"sync"
	"sync/atomic"
	"time"
)

type metricas struct {
	mu     sync.Mutex
	soma   map[string]time.Duration
	chamadas map[string]int
}

func (m *metricas) registrar(rota string, d time.Duration) {
	m.mu.Lock()
	defer m.mu.Unlock()
	m.soma[rota] += d
	m.chamadas[rota]++
}

func (m *metricas) media(rota string) time.Duration {
	m.mu.Lock()
	defer m.mu.Unlock()
	if m.chamadas[rota] == 0 {
		return 0
	}
	return m.soma[rota] / time.Duration(m.chamadas[rota])
}

var proximoReq atomic.Int64
var m = &metricas{soma: map[string]time.Duration{}, chamadas: map[string]int{}}

func comRequestID(proximo http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		id := fmt.Sprintf("req-%d", proximoReq.Add(1))
		inicio := time.Now()
		proximo.ServeHTTP(w, r)
		dur := time.Since(inicio)
		m.registrar(r.URL.Path, dur)
		slog.Info("requisicao", "request_id", id, "rota", r.URL.Path,
			"duracao_ms", float64(dur.Microseconds())/1000.0)
	})
}

func main() {
	mux := http.NewServeMux()
	mux.HandleFunc("/ola", func(w http.ResponseWriter, r *http.Request) {
		fmt.Fprint(w, "ola")
	})
	mux.HandleFunc("/healthz", func(w http.ResponseWriter, r *http.Request) {
		w.WriteHeader(http.StatusOK)
		fmt.Fprint(w, "ok")
	})
	servidor := httptest.NewServer(comRequestID(mux))
	defer servidor.Close()

	// Profile de CPU sob carga gerada, nao esperada.
	arqProfile := filepath.Join(os.TempDir(), "aula24_cpu.prof")
	f, err := os.Create(arqProfile)
	if err != nil {
		panic(err)
	}
	if err := pprof.StartCPUProfile(f); err != nil {
		panic(err)
	}
	cliente := &http.Client{Timeout: 2 * time.Second}
	for i := 0; i < 100; i++ {
		resp, err := cliente.Get(servidor.URL + "/ola")
		if err != nil {
			panic(err)
		}
		io.Copy(io.Discard, resp.Body)
		resp.Body.Close()
	}
	resp, err := cliente.Get(servidor.URL + "/healthz")
	if err != nil {
		panic(err)
	}
	corpoSaude, _ := io.ReadAll(resp.Body)
	resp.Body.Close()
	pprof.StopCPUProfile()
	f.Close()
	info, _ := os.Stat(arqProfile)

	fmt.Println("healthz:", resp.StatusCode, string(corpoSaude))
	fmt.Println("chamadas /ola:", m.chamadas["/ola"], "| media_us:", m.media("/ola").Microseconds())
	fmt.Println("profile bytes:", info.Size(), "| inspecione com: go tool pprof -top <binario>", filepath.Base(arqProfile))
	_ = strings.TrimSpace // logs acima ja carregam request_id, rota e duracao
	fmt.Println("testes ok")
}
```

4. O profile e gerado de verdade (arquivo nao vazio em `/tmp`) e se le com `go tool pprof -top <binario> aula24_cpu.prof`: as funcoes no topo sao onde a CPU foi, nao onde o codigo e feio. Sob 100 requisicoes triviais, quem aparece e a propria maquina HTTP — profile de servico ocioso mede o runtime, nao o seu gargalo.

5. Campos minimos numa rota critica: `request_id`, rota, metodo, status, duracao, e o identificador do recurso afetado (nunca segredos nem payload). Sem `request_id`, o log de uma requisicao se perde no log das outras mil.

6. Quatro cenarios: "o que aconteceu com **esta** requisicao" e **log**; "o servico esta piorando esta semana" e **metrica**; "qual servico da cadeia esta lento" e **trace**; "por que este processo consome CPU/memoria" e **pprof**. Trocar um pelo outro e responder a pergunta errada com a ferramenta certa.

**Autoverificacao:** 100 chamadas contadas, `/healthz` 200, profile nao vazio e cada log com `request_id`.
