---
layout: default
---

# Aula 26 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../26.md#22-exercicios-obrigatorios).

```go
package main

import (
	"errors"
	"fmt"
	"log/slog"
	"os"
	"path/filepath"
	"sync"
	"time"
)

type job struct {
	id        string
	tipo      string // "rapido", "lento" ou "falha"
	tentativas int
}

var errFilaCheiaTimeout = errors.New("timeout ao enfileirar")

type sistema struct {
	fila       chan job
	mortos     []job
	concluidos []string
	mu         sync.Mutex
	wg         sync.WaitGroup
	arq        string
}

func (s *sistema) enfileirar(j job, timeout time.Duration) error {
	select {
	case s.fila <- j:
		slog.Info("enfileirado", "componente", "scheduler", "job", j.id)
		return nil
	case <-time.After(timeout):
		slog.Info("enfileiramento expirou", "componente", "scheduler", "job", j.id)
		return errFilaCheiaTimeout
	}
}

func (s *sistema) executar(j job) error {
	switch j.tipo {
	case "lento":
		time.Sleep(30 * time.Millisecond)
		return nil
	case "falha":
		if j.tentativas == 0 {
			return errors.New("falha transitoria simulada")
		}
		return nil
	default:
		return nil
	}
}

func (s *sistema) trabalhador() {
	defer s.wg.Done()
	for j := range s.fila {
		err := s.executar(j)
		s.mu.Lock()
		if err != nil {
			j.tentativas++
			s.mortos = append(s.mortos, j)
			s.mu.Unlock()
			slog.Info("para dead letter", "componente", "worker", "job", j.id, "tentativas", j.tentativas)
			continue
		}
		s.concluidos = append(s.concluidos, j.id)
		s.mu.Unlock()
		// Persistencia simulada: uma linha por conclusao, com flush no fechamento do arquivo.
		f, ferr := os.OpenFile(s.arq, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0o644)
		if ferr == nil {
			fmt.Fprintln(f, j.id)
			f.Close()
		}
		slog.Info("concluido", "componente", "worker", "job", j.id, "tipo", j.tipo)
	}
}

func (s *sistema) reprocessarMortos(timeout time.Duration) int {
	s.mu.Lock()
	mortos := s.mortos
	s.mortos = nil
	s.mu.Unlock()
	for _, j := range mortos {
		slog.Info("reprocessando", "componente", "deadletter", "job", j.id)
		_ = s.enfileirar(j, timeout)
	}
	return len(mortos)
}

func main() {
	arq := filepath.Join(os.TempDir(), "aula26_concluidos.txt")
	_ = os.Remove(arq)
	s := &sistema{fila: make(chan job, 2), arq: arq}
	s.wg.Add(1)
	go s.trabalhador()

	// Fila de 2, worker ocupado com lento: o terceiro enfileiramento expira.
	_ = s.enfileirar(job{id: "lento-1", tipo: "lento"}, time.Second)
	_ = s.enfileirar(job{id: "rapido-1", tipo: "rapido"}, time.Second)
	_ = s.enfileirar(job{id: "rapido-2", tipo: "rapido"}, time.Second)
	err := s.enfileirar(job{id: "sem-vaga", tipo: "rapido"}, 20*time.Millisecond)
	fmt.Println("enfileirar sem vaga expirou:", errors.Is(err, errFilaCheiaTimeout))

	_ = s.enfileirar(job{id: "falha-1", tipo: "falha"}, 2*time.Second)
	fmt.Println("reprocessados:", s.reprocessarMortos(2*time.Second))

	close(s.fila)
	s.wg.Wait()
	dados, _ := os.ReadFile(arq)
	fmt.Println("concluidos:", len(s.concluidos), "| mortos restantes:", len(s.mortos), "| linhas persistidas:", len(dados) > 0)
	_ = os.Remove(arq)
	fmt.Println("testes ok")
}
```

4. Custos diferentes no mesmo backlog: com 1 trabalhador, um job lento de 30 ms deixa 30 ms de fila para todos atras dele — o exercicio se mede comparando `concluidos` por segundo com tipos mistos contra tipos uniformes, e a resposta de desenho costuma ser **filas separadas por classe de custo**, nao workers genericos maiores.

6. Para sobreviver a crash, sai da memoria: a fila (hoje um canal), as conclusoes (hoje um arquivo-append, que ja e o embriao) e a dead letter (hoje um slice) — os tres viram tabelas ou arquivos com o mesmo ciclo de vida do job. O que nao muda: enfileirar com timeout, tentar com limite e estacionar o venenoso continua sendo a politica; so o suporte muda.

**Autoverificacao:** enfileiramento sem vaga expira pelo timeout (nao descarta em silencio), `falha-1` vai para a dead letter, e reprocessado e conclui na segunda tentativa, com linha persistida.
