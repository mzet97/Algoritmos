---
layout: default
---

# Aula 19 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../19.md#15-exercicios-obrigatorios).

```go
package main

import (
	"context"
	"fmt"
	"log/slog"
	"os"
	"strconv"
	"time"
)

type config struct {
	url     string
	timeout time.Duration
	retries int
}

func carregarConfig() (config, error) {
	timeout := 2 * time.Second
	if bruto := os.Getenv("AULA19_TIMEOUT_MS"); bruto != "" {
		ms, err := strconv.Atoi(bruto)
		if err != nil || ms <= 0 {
			return config{}, fmt.Errorf("AULA19_TIMEOUT_MS invalido: %q", bruto)
		}
		timeout = time.Duration(ms) * time.Millisecond
	}
	return config{
		url:     envOu("AULA19_URL", "https://exemplo.invalid/saude"),
		timeout: timeout,
		retries: 2,
	}, nil
}

func envOu(nome, padrao string) string {
	if v := os.Getenv(nome); v != "" {
		return v
	}
	return padrao
}

// executar e a funcao principal separada: recebe config e contexto, devolve erro.
func executar(ctx context.Context, cfg config, trabalho func(context.Context) error) error {
	ctx, cancel := context.WithTimeout(ctx, cfg.timeout)
	defer cancel()
	inicio := time.Now()
	slog.Info("job iniciado", "url", cfg.url, "timeout_ms", cfg.timeout.Milliseconds())
	err := trabalho(ctx)
	slog.Info("job terminado", "duracao_ms", time.Since(inicio).Milliseconds(), "erro", err != nil)
	return err
}

func main() {
	cfg, err := carregarConfig()
	if err != nil {
		slog.Error("config invalida", "erro", err)
		os.Exit(2)
	}
	rapido := func(ctx context.Context) error {
		select {
		case <-time.After(10 * time.Millisecond):
			return nil
		case <-ctx.Done():
			return ctx.Err()
		}
	}
	lento := func(ctx context.Context) error {
		select {
		case <-time.After(10 * time.Second):
			return nil
		case <-ctx.Done():
			return ctx.Err()
		}
	}
	fmt.Println("job rapido:", executar(context.Background(), cfg, rapido) == nil)
	curto := cfg
	curto.timeout = 30 * time.Millisecond
	err = executar(context.Background(), curto, lento)
	fmt.Println("job lento cortado pelo contexto:", err == context.DeadlineExceeded)
	fmt.Println("testes ok")
}
```

3. Logs essenciais de um job automatizado: inicio com os parametros que importam (alvo, timeout, tentativa), fim com duracao e desfecho, e erro com a causa embrulhada (`%w`) — o bastante para responder "rodou? com o que? como acabou?" sem abrir o codigo.

4. Checklist operacional de um servidor pequeno: sobe com config invalida falhando rapido? `/healthz` responde sem dependencia externa? `SIGTERM` encerra limpo (aula 20)? Logs tem `request_id` (aula 24)? Limites de corpo/tempo existem? Cada item vira teste ou alarme, nao desejo.

5. O comando local de prontidao, nesta ordem:

    go fmt ./... && go vet ./... && go test ./... && go test -race ./...

`fmt` primeiro porque diff de formatacao nao se mistura com diff de logica; `-race` por ultimo porque e o mais lento — e o que mais encontra.

**Autoverificacao:** config invalida sairia com codigo 2, job rapido termina, job lento e cortado pelo `DeadlineExceeded` do contexto.
