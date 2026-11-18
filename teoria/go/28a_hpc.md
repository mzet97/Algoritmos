---
layout: default
---

# Aula 28a - Benchmark honesto, escala e o preço da contenção

---

## Objetivo da Aula

Medir um programa paralelo em Go com método — hipótese, baseline correto, aquecimento, mínimo de repetições, variância, ambiente anotado — e explicar speedup e seus limites com a lei de Amdahl e o custo da contenção. Pré-requisitos: aulas 08a, 08b, 11a, 14 e 17. Não precisa de cluster, GPU, MPI nem contadores de hardware.

Ao final, você deverá: construir benchmark honesto com linha de base, medir escala multicore, distinguir contenção real de granularidade, e emitir veredito numérico.
Mediremos com níveis de proficiência:
- **Nível 1 — Recordar:** `definir linha de base, speedup e contenção`;
- **Nível 2 — Aplicar:** `diagnosticar benchmark inválido e corrigir a metodologia`;
- **Nível 3 — Projetar:** `projetar um experimento justificando hipótese e veredito numérico`.

## 1. O protocolo, antes do cronômetro

1. **Hipótese escrita:** o que deve melhorar, por quê, e quanto.
2. **Baseline correto:** soma sequencial verificada contra fórmula fechada. Benchmark de programa errado mede a velocidade do erro.
3. **Aquecimento fora da conta:** primeira execução aquece cache, GC e turbo.
4. **Mínimo de N rodadas:** o mínimo filtra ruído para baixo; a média esconde bimodalidade. Reporte dispersão, não um dígito.
5. **Ambiente anotado:** `GOMAXPROCS`, CPUs, Go, SO, carga. Sem isso o número não é reproduzível — é anedota.

## 2. Amdahl antes de medir

Com fração paralelizável `p` e N trabalhadores, o speedup máximo é `1 / ((1-p) + p/N)` — e isso *antes* de contar contenção, comunicação e desbalanceamento. Para soma particionada sem dependências, `p ≈ 1` e o teto prático vem da banda de memória, não da lei: 8 núcleos famintos dividem o mesmo barramento.

## 3. Contenção: somar junto custa; somar separado escala

Oito goroutines incrementando o *mesmo* `atomic.Int64` disputam uma linha de cache: correto, mas lento — contenção, não paralelismo. Oito partições independentes com redução final somam em paralelo de verdade. O exemplo mede os três desenhos com a mesma invariante (total exato) e deixa o cronômetro dizer qual é qual.

## 4. O modelo de custo do acelerador

`t_total = t_ida + t_kernel + t_volta`, com banda declarada. Sem a conta da viagem, "GPU acelera Nx" é fantasia. A regra honesta: só atravessa o barramento quem já provou que o cálculo local é o gargalo — e o cálculo local precisa de número, não de pressentimento.

## 5. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/28a_hpc.go) contém exatamente o programa abaixo. Só biblioteca padrão (`go vet` limpo, pronto para `-race`):

```bash
go vet teoria/go/exemplos/28a_hpc.go && go run teoria/go/exemplos/28a_hpc.go
```

```go
package main

import (
	"fmt"
	"runtime"
	"sync"
	"sync/atomic"
	"time"
)

const n = 20_000_000
const repeticoes = 5

// medir devolve o minimo de N rodadas: filtra ruido para baixo.
func medir(repeticoes int, f func() int64) (int64, float64) {
	var melhor float64 = 1e30
	var resultado int64
	for i := 0; i < repeticoes; i++ {
		inicio := time.Now()
		resultado = f()
		if dt := time.Since(inicio).Seconds(); dt < melhor {
			melhor = dt
		}
	}
	return resultado, melhor
}

func somaSeq(v []int64) int64 {
	var s int64
	for _, x := range v {
		s += x
	}
	return s
}

func somaParalela(v []int64, workers int) int64 {
	parciais := make([]int64, workers)
	var wg sync.WaitGroup
	for w := 0; w < workers; w++ {
		wg.Add(1)
		go func(w int) {
			defer wg.Done()
			ini := (len(v) * w) / workers
			fim := (len(v) * (w + 1)) / workers
			var s int64
			for _, x := range v[ini:fim] {
				s += x
			}
			parciais[w] = s
		}(w)
	}
	wg.Wait()
	var total int64
	for _, p := range parciais {
		total += p
	}
	return total
}

// somaContendida: todas as goroutines no MESMO atomico (correto e lento).
func somaContendida(v []int64, workers int) int64 {
	var total atomic.Int64
	var wg sync.WaitGroup
	for w := 0; w < workers; w++ {
		wg.Add(1)
		go func(w int) {
			defer wg.Done()
			ini := (len(v) * w) / workers
			fim := (len(v) * (w + 1)) / workers
			for _, x := range v[ini:fim] {
				total.Add(x)
			}
		}(w)
	}
	wg.Wait()
	return total.Load()
}

func main() {
	fmt.Printf("n=%d gcpus=%d gomaxprocs=%d\n", n, runtime.NumCPU(), runtime.GOMAXPROCS(0))
	v := make([]int64, n)
	for i := range v {
		v[i] = int64(i % 97)
	}
	esperado := somaSeq(v)

	// Aquecimento fora da conta.
	_ = somaParalela(v, 2)

	tot, tSeq := medir(repeticoes, func() int64 { return somaSeq(v) })
	if tot != esperado {
		panic("baseline incorreto")
	}
	fmt.Printf("seq: %.3fs\n", tSeq)

	for _, w := range []int{2, 4, 8} {
		tot, t := medir(repeticoes, func() int64 { return somaParalela(v, w) })
		if tot != esperado {
			panic("variante incorreta")
		}
		fmt.Printf("par%d: %.3fs speedup=%.2fx\n", w, t, tSeq/t)
	}

	tot, tCon := medir(repeticoes, func() int64 { return somaContendida(v, 4) })
	if tot != esperado {
		panic("contendido incorreto")
	}
	fmt.Printf("contendido4: %.3fs (correto, mas serializado pela linha de cache)\n", tCon)

	// Conta do acelerador: aritmetica deterministica, sem hardware.
	const bytesTotais = float64(n * 8)
	const bandaGBs = 16.0
	const tKernel = 0.001
	tViagem := 2 * (bytesTotais / (bandaGBs * 1024 * 1024 * 1024))
	fmt.Printf("viagem=%.4fs kernel=%.3fs total=%.4fs\n", tViagem, tKernel, tViagem+tKernel)
	if tViagem+tKernel < tSeq {
		fmt.Println("veredito: a GPU hipotetica venceria NESTE cenario")
	} else {
		fmt.Println("veredito: fique na CPU; a viagem custa mais que o calculo")
	}
	fmt.Println("hpc ok: baseline verificado, escala medida, conta feita")
}
```

Saída esperada (tempos variam por máquina, carga e GOMAXPROCS):

```text
n=20000000 gcpus=N gomaxprocs=N
seq: ~s
par2: ~s speedup=~x
par4: ~s speedup=~x
par8: ~s speedup=~x
contendido4: ~s (correto, mas serializado pela linha de cache)
viagem=0.0186s kernel=0.001s total=0.0196s
veredito: ...
hpc ok: baseline verificado, escala medida, conta feita
```

Leia com o modelo certo: soma particionada de 160 MB tende a escalar até a banda saturar; a curva par2/par4/par8 mostra onde. O compartilhado prova o ponto oposto: invariante intacta, desempenho de contenção. O veredito da GPU sai da aritmética — com 160 MB e 16 GiB/s, a viagem custa ~19 ms; contra kernel de 1 ms e CPU de ~tens de ms, a conta decide, não o marketing. E o veredito muda com o ambiente: na build normal desta máquina ele diz "fique na CPU"; sob `-race` (que desacelera o sequencial ~10x), a mesma aritmética diz "venceria". O veredito nunca foi uma propriedade do código — é uma função do ambiente medido.

## 6. Erros comuns e implicações defensivas

1. **Medir uma vez:** turbo, GC e vizinhos dançam. Mínimo de N rodadas + ambiente anotado.
2. **Medir sem invariante:** cada variante afirma igualdade com a baseline. Sem isso, o benchmark pode cronometrar o erro.
3. **Confundir contenção com paralelismo:** some partições independentes; some no mesmo `atomic` só para medir o custo da briga.
4. **Alegar GPU sem a viagem:** some ida e volta com banda declarada. Sem isso, fantasia.
5. **Prometer escala linear:** Amdahl + banda + contenção dizem não antes de você medir. Preveja, depois confira.

## 7. Ponte com as demais trilhas

C mede o mesmo com pthreads e `perf` (aulas 28a/30a); Python compara NumPy, threads e processos (aula 29a); Rust compara escalar e paralelo antes de `wgpu` (aula R7 futura). O método — hipótese, baseline, mínimo, ambiente — é o mesmo em todas.

## 8. Exercícios obrigatórios

1. Varie `GOMAXPROCS` (1, 2, 4, 8) com workers fixos em 8 e explique a curva com a lei de Amdahl.
2. Troque `int64` por `byte` e preveja o efeito na escala antes de medir. Confirme ou refute.
3. Implemente a versão com `string` concatenada por worker e meça o custo da alocação contra a soma numérica.
4. Refaça a conta da GPU com banda de 100 GiB/s e kernel de 10 ms. O veredito muda? Mostre a conta.

Use [pistas graduais](exercicios/28a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/28a_solucoes.md) depois de registrar sua própria explicação.

## 9. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- enunciar hipótese, baseline, aquecimento, repetição e ambiente antes de cronometrar;
- explicar por que a soma particionada escala até a banda e o `atomic` compartilhado não;
- prever a curva de speedup com Amdahl e confrontar com a medição;
- fazer a conta da viagem e emitir o veredito com os números;
- rodar `go vet` limpo e, sob `-race`, confirmar ausência de data race.

**Próxima aula:** `cgo` avançado e ABI. O raciocínio desta aula — medir antes de atravessar fronteiras — decide se chamar C vale a pena.

---

### Como esta aula será avaliada

| Critério | Peso |
| -------- | ---: |
| Conceitos e vocabulário (nível 1) | 20% |
| Previsão de comportamento e diagnóstico de erros (nível 2) | 40% |
| Projeto e justificativa de decisões (nível 3) | 30% |
| Registro de comandos, saídas e interpretação | 10% |

**Aprovação:** nota mínima 7,0; pelo menos 60% dos pontos devem vir de previsão e diagnóstico (níveis 2 e 3), não apenas de código que executa.

**Para se aprofundar:**

1. *Computer Organization and Design: The Hardware/Software Interface, RISC-V Edition*, Patterson e Hennessy (Morgan Kaufmann, ISBN 978-0128203316); 2. Documentação oficial em [go.dev/doc](https://go.dev/doc/); 3. Manual do programador Linux (`man 2 mmap`, `man 2 syscall`).
