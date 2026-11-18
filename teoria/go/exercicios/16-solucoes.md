---
layout: default
---

# Aula 16 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../16.md#13-exercicios-obrigatorios).

```go
package main

import (
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"net/http/httptest"
	"strings"
	"time"
)

type RespostaJSON struct {
	Status  string `json:"status"`
	Versao  int    `json:"versao"`
}

type Pedido struct {
	Nome string `json:"nome"`
}

func handlerPing(w http.ResponseWriter, r *http.Request) {
	w.Write([]byte("pong"))
}

func handlerStatus(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(RespostaJSON{Status: "ok", Versao: 1})
}

func handlerEco(w http.ResponseWriter, r *http.Request) {
	var pedido Pedido
	if err := json.NewDecoder(r.Body).Decode(&pedido); err != nil {
		http.Error(w, "json invalido", http.StatusBadRequest)
		return
	}
	if strings.TrimSpace(pedido.Nome) == "" {
		http.Error(w, "campo nome obrigatorio", http.StatusBadRequest)
		return
	}
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(map[string]string{"ola": pedido.Nome})
}

func novoMux() *http.ServeMux {
	mux := http.NewServeMux()
	mux.HandleFunc("/ping", handlerPing)
	mux.HandleFunc("/status", handlerStatus)
	mux.HandleFunc("/eco", handlerEco)
	return mux
}

func main() {
	servidor := httptest.NewServer(novoMux())
	defer servidor.Close()

	cliente := &http.Client{Timeout: 2 * time.Second}

	resp, err := cliente.Get(servidor.URL + "/ping")
	if err != nil {
		panic(err)
	}
	corpo, _ := io.ReadAll(resp.Body)
	resp.Body.Close()
	fmt.Printf("GET /ping -> %d %s\n", resp.StatusCode, corpo)

	resp, err = cliente.Get(servidor.URL + "/status")
	if err != nil {
		panic(err)
	}
	var status RespostaJSON
	json.NewDecoder(resp.Body).Decode(&status)
	resp.Body.Close()
	fmt.Printf("GET /status -> %d %+v\n", resp.StatusCode, status)

	resp, err = cliente.Post(servidor.URL+"/eco", "application/json",
		strings.NewReader(`{"nome":"Ada"}`))
	if err != nil {
		panic(err)
	}
	corpo, _ = io.ReadAll(resp.Body)
	resp.Body.Close()
	fmt.Printf("POST /eco -> %d %s", resp.StatusCode, corpo)

	resp, err = cliente.Post(servidor.URL+"/eco", "application/json",
		strings.NewReader(`{"nome":""}`))
	if err != nil {
		panic(err)
	}
	resp.Body.Close()
	fmt.Println("POST /eco vazio ->", resp.StatusCode)
	if resp.StatusCode != http.StatusBadRequest {
		panic("validacao nao respondeu 400")
	}
	fmt.Println("testes ok")
}
```

O programa sobe o servidor de teste (`httptest`) e o chama com um cliente de verdade com timeout de 2s — handler e cliente conferidos de ponta a ponta sem porta manual. Em producao, `main` separa: `novoMux()` monta as rotas e `http.ListenAndServe(":8080", novoMux())` sobe; os handlers nao mudam.

3. Validar na borda: nome vazio responde `400` com mensagem, nao `200` com um eco vazio. Erro do cliente e 4xx; erro nosso e 5xx — trocar os dois e ensinar o cliente a tentar de novo o que nunca vai funcionar.

**Autoverificacao:** `pong` no `/ping`, JSON tipado no `/status`, `400` no nome vazio e cliente com `Timeout` explicito.
