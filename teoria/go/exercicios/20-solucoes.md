---
layout: default
---

# Aula 20 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../20.md#18-exercicios-obrigatorios).

```go
package main

import (
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"net/http"
	"net/http/httptest"
	"strings"
	"time"
)

func escreverJSON(w http.ResponseWriter, status int, valor any) {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(status)
	json.NewEncoder(w).Encode(valor)
}

func handlerVersion(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		w.Header().Set("Allow", http.MethodGet)
		http.Error(w, "metodo nao permitido", http.StatusMethodNotAllowed)
		return
	}
	escreverJSON(w, http.StatusOK, map[string]string{
		"nome":   "servico-aula20",
		"versao": "1.0.0",
	})
}

func handlerEco(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		w.Header().Set("Allow", http.MethodPost)
		http.Error(w, "metodo nao permitido", http.StatusMethodNotAllowed)
		return
	}
	r.Body = http.MaxBytesReader(w, r.Body, 16)
	corpo, err := io.ReadAll(r.Body)
	if err != nil {
		http.Error(w, "corpo acima do limite", http.StatusRequestEntityTooLarge)
		return
	}
	escreverJSON(w, http.StatusOK, map[string]string{"eco": strings.ToUpper(string(corpo))})
}

func exigeToken(proximo http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		if r.Header.Get("X-Token") != "segredo-de-laboratorio" {
			http.Error(w, "token ausente ou invalido", http.StatusUnauthorized)
			return
		}
		proximo.ServeHTTP(w, r)
	})
}

func novoServidor() http.Handler {
	mux := http.NewServeMux()
	mux.HandleFunc("/version", handlerVersion)
	mux.Handle("/eco", exigeToken(http.HandlerFunc(handlerEco)))
	return mux
}

func main() {
	servidor := httptest.NewServer(novoServidor())
	defer servidor.Close()
	cliente := &http.Client{Timeout: 2 * time.Second}

	get := func(caminho string) (int, string) {
		resp, err := cliente.Get(servidor.URL + caminho)
		if err != nil {
			panic(err)
		}
		defer resp.Body.Close()
		corpo, _ := io.ReadAll(resp.Body)
		return resp.StatusCode, string(corpo)
	}
	post := func(caminho, corpoTexto string, comToken bool) (int, string) {
		req, _ := http.NewRequest(http.MethodPost, servidor.URL+caminho, strings.NewReader(corpoTexto))
		if comToken {
			req.Header.Set("X-Token", "segredo-de-laboratorio")
		}
		resp, err := cliente.Do(req)
		if err != nil {
			panic(err)
		}
		defer resp.Body.Close()
		corpo, _ := io.ReadAll(resp.Body)
		return resp.StatusCode, string(corpo)
	}

	codigo, corpo := get("/version")
	fmt.Printf("GET /version -> %d %s", codigo, corpo)
	codigo, _ = post("/version", "", false)
	fmt.Println("POST /version ->", codigo, "(esperado 405)")
	codigo, _ = post("/eco", "ola", false)
	fmt.Println("POST /eco sem token ->", codigo, "(esperado 401)")
	codigo, corpo = post("/eco", "ola", true)
	fmt.Printf("POST /eco com token -> %d %s", codigo, corpo)
	codigo, _ = post("/eco", strings.Repeat("x", 40), true)
	fmt.Println("POST /eco corpo grande ->", codigo, "(esperado 413)")

	// Shutdown gracioso sobre um servidor real, com prazo.
	real := &http.Server{Addr: "127.0.0.1:18099", Handler: novoServidor()}
	go func() {
		_ = real.ListenAndServe()
	}()
	time.Sleep(100 * time.Millisecond)
	ctx, cancel := context.WithTimeout(context.Background(), 2*time.Second)
	defer cancel()
	err := real.Shutdown(ctx)
	fmt.Println("shutdown limpo:", err == nil, "| erro de contexto:", errors.Is(err, context.DeadlineExceeded))
	if err != nil {
		panic(err)
	}
	fmt.Println("testes ok")
}
```

O shutdown e demonstrado de verdade: `Shutdown` fecha os listeners, espera as conexoes ativas terminarem (aqui, nenhuma) e devolve `nil` dentro do prazo — `Close()` seria o machado (corta tudo), `Shutdown` e a saida educada. Num `main` de producao, quem chama e o tratador de `SIGINT`/`SIGTERM`.

3 e 4. `MaxBytesReader` nao rejeita na porta: ele faz a **leitura** falhar ao passar de 16 bytes, e o handler traduz para `413`. Metodo incorreto e `405` com `Allow` — nao `404`, porque a rota existe; quem errou foi o verbo.

**Autoverificacao:** 401 sem token, 413 no corpo grande, 405 no verbo errado e um shutdown `nil` observado com servidor real.
