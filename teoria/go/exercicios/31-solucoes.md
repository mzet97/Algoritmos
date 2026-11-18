---
layout: default
---

# Aula 31 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../31.md#20-exercicios-obrigatorios).

```go
package main

import (
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"net/http"
	"net/http/httptest"
	"os"
	"path/filepath"
	"strings"
	"time"
)

type check struct {
	Nome       string `json:"nome"`
	URL        string `json:"url"`
	Arquivo    string `json:"arquivo"`
	Severidade string `json:"severidade"` // "warning" ou "critical"
}

type resultado struct {
	Nome       string
	OK         bool
	Severidade string
	Duracao    time.Duration
}

func codigoDeSaida(resultados []resultado) int {
	codigo := 0
	for _, r := range resultados {
		if r.OK {
			continue
		}
		if r.Severidade == "critical" {
			return 2
		}
		codigo = 1
	}
	return codigo
}

func verificarHTTP(cliente *http.Client, c check) resultado {
	inicio := time.Now()
	var resp *http.Response
	var err error
	for tentativas := 0; tentativas < 3; tentativas++ {
		resp, err = cliente.Get(c.URL) // GET: idempotente, retry permitido
		if err == nil {
			break
		}
		time.Sleep(time.Duration(tentativas+1) * 10 * time.Millisecond) // backoff
	}
	ok := err == nil && resp != nil && resp.StatusCode == http.StatusOK
	if resp != nil {
		io.Copy(io.Discard, resp.Body)
		resp.Body.Close()
	}
	return resultado{Nome: c.Nome, OK: ok, Severidade: c.Severidade, Duracao: time.Since(inicio)}
}

func renderPrometheus(resultados []resultado) string {
	var sb strings.Builder
	for _, r := range resultados {
		valor := 0
		if r.OK {
			valor = 1
		}
		fmt.Fprintf(&sb, "check_ok{nome=%q} %d\n", r.Nome, valor)
	}
	return sb.String()
}

func main() {
	saudavel := httptest.NewServer(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		fmt.Fprint(w, "ok")
	}))
	defer saudavel.Close()
	lento := httptest.NewServer(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		time.Sleep(300 * time.Millisecond)
		fmt.Fprint(w, "tarde demais")
	}))
	defer lento.Close()

	arqCfg := filepath.Join(os.TempDir(), "aula31_checks.json")
	cfg := []check{
		{Nome: "saudavel", URL: saudavel.URL, Severidade: "critical"},
		{Nome: "lento", URL: lento.URL, Severidade: "warning"},
	}
	dados, _ := json.Marshal(cfg)
	if err := os.WriteFile(arqCfg, dados, 0o644); err != nil {
		panic(err)
	}
	defer os.Remove(arqCfg)

	bruto, err := os.ReadFile(arqCfg)
	if err != nil {
		panic(err)
	}
	var checks []check
	if err := json.Unmarshal(bruto, &checks); err != nil {
		panic(err)
	}
	fmt.Println("checks carregados do JSON:", len(checks))

	cliente := &http.Client{Timeout: 100 * time.Millisecond}
	var resultados []resultado
	for _, c := range checks {
		resultados = append(resultados, verificarHTTP(cliente, c))
	}
	for _, r := range resultados {
		fmt.Println(r.Nome, "ok:", r.OK, "severidade:", r.Severidade)
	}
	fmt.Println("codigo de saida (warning falho, critical ok):", codigoDeSaida(resultados))
	fmt.Print(renderPrometheus(resultados))
	_ = errors.Is // severidade desconhecida cairia no ramo warning: valide na carga em producao
	fmt.Println("testes ok")
}
```

Leitura: o lento estoura os 100 ms do cliente e falha como `warning`; o saudavel passa. Codigo de saida **1** — se o saudavel (critical) falhasse, seria **2**. Essa graduacao e o que permite ao agendador alarmar diferente para "degradado" e "morto".

6. Benchmark do runner: varie **uma** dimensao por vez (workers com checks fixos, depois checks com workers fixos), como na [aula 14](14-solucoes.md) — variar as duas juntas produz um numero que nao responde a nenhuma pergunta.

**Autoverificacao:** config JSON carregado, lento falha por timeout com retry de GET, codigo 1 para warning e o formato Prometheus renderiza os mesmos resultados.
