---
layout: default
---

# Aula 08b - Máquina, memória e sistema operacional vistos de Go

---

## Nesta aula

Explicar o caminho de um programa Go entre código-fonte, runtime, chamadas de sistema e hardware — e investigar cada camada com ferramentas do espaço de usuário, sem sair da biblioteca padrão. Pré-requisitos: aulas 01 a 08a. Não precisa saber escape analysis, `cgo`, concorrência nem benchmarks formais.

Você vai aprender a interpretar uso de memória e tempo sem inferir causas microarquiteturais, distinguir medição de alegação, e apontar o que o runtime observa.
---

## 1. O runtime é um SO em miniatura

Um binário Go não fala direto com o kernel para tudo: o runtime agenda goroutines sobre threads do SO, coleta lixo, cresce a pilha e escolhe quantas threads executam código de usuário ao mesmo tempo (`GOMAXPROCS`, por padrão o número de CPUs). `runtime.NumCPU()` pergunta ao SO quantas CPUs há; `runtime.GOMAXPROCS(0)` lê o limite atual sem alterá-lo. Em contêiner com quota de CPU, esses dois números podem divergir do que o programa realmente pode usar — leia a documentação do ambiente em vez de presumir.

## 2. Processo, descritores e bloqueio

`os.Getpid()` devolve o PID; `os.StartProcess` cria um processo filho com redirecionamento explícito e `Process.Wait` colhe a saída (o equivalente Go do `waitpid` da aula C 16a). Esquecer `Wait` retém estado do filho — o mesmo zumbi, com outro nome. Arquivos abrem como descritores (`os.Open`); ler em blocos (`f.Read(buf)`) faz menos travessias que ler byte a byte. Erro de arquivo ausente ou sem permissão é resposta normal do SO: trate `os.IsNotExist`/`os.IsPermission`, não presuma sucesso.

## 3. Memória virtual, páginas e os dois medidores

O processo enxerga endereços virtuais; o SO mapeia páginas sob demanda. Duas réguas diferentes:

- `runtime.ReadMemStats`: heap alocado, objetos, pausas de GC — visão do runtime, não do SO.
- RSS do processo (via `/proc/self/statm` no Linux): pico real, incluindo runtime, pilhas e páginas.

Heap do runtime <= RSS, sempre. Se o RSS cresce e `HeapAlloc` não, a memória está fora do heap gerenciado — pilhas, `mmap` direto ou `cgo`. `os.Getpagesize()` informa o tamanho de página sem presumir 4096.

## 4. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/08b_sistema.go) contém exatamente o programa abaixo. Só biblioteca padrão, portátil com recuos honestos onde o SO não oferece o dado:

```bash
go vet teoria/go/exemplos/08b_sistema.go && go run teoria/go/exemplos/08b_sistema.go
```

```go
package main

import (
	"fmt"
	"os"
	"runtime"
	"strings"
	"time"
)

// reguaSyscall cronometra os.Getpid: custo minimo de travessia ao kernel.
func reguaSyscall(repeticoes int) float64 {
	inicio := time.Now()
	for i := 0; i < repeticoes; i++ {
		_ = os.Getpid()
	}
	return time.Since(inicio).Seconds() / float64(repeticoes)
}

// lerMapa conta regioes e executaveis em /proc/self/maps (Linux).
// Fora do Linux, devolve zeros e diz por que.
func lerMapa() (total, exec int, origem string) {
	dados, err := os.ReadFile("/proc/self/maps")
	if err != nil {
		return 0, 0, "indisponivel fora do Linux"
	}
	for _, linha := range strings.Split(string(dados), "\n") {
		if linha == "" {
			continue
		}
		total++
		if strings.Contains(linha, "r-xp") {
			exec++
		}
	}
	return total, exec, "/proc/self/maps"
}

func relatorioMemoria(rotulo string) {
	var m runtime.MemStats
	runtime.ReadMemStats(&m)
	fmt.Printf("%s: heap_mb=%.2f objetos=%d gcs=%d pausa_total_ms=%.2f\n",
		rotulo,
		float64(m.HeapAlloc)/1024/1024,
		m.HeapObjects,
		m.NumGC,
		float64(m.PauseTotalNs)/1e6)
}

func main() {
	fmt.Printf("cpus=%d gomaxprocs=%d pid=%d pagina=%d\n",
		runtime.NumCPU(), runtime.GOMAXPROCS(0), os.Getpid(), os.Getpagesize())

	const rep = 1000000
	fmt.Printf("getpid: %.1f ns/chamada (%d repeticoes)\n", reguaSyscall(rep)*1e9, rep)

	relatorioMemoria("antes")
	bloco := make([]byte, 8<<20) // 8 MiB gerenciados
	for i := 0; i < len(bloco); i += os.Getpagesize() {
		bloco[i] = 1 // toca uma pagina por vez
	}
	relatorioMemoria("com-bloco-vivo") // ~8 MiB: bloco referenciado abaixo
	runtime.KeepAlive(bloco)           // vida ate aqui; depois, coletavel
	runtime.GC()
	relatorioMemoria("apos-gc") // cai: nada mais referencia o bloco

	total, exec, origem := lerMapa()
	fmt.Printf("regioes=%d executaveis=%d origem=%s\n", total, exec, origem)
	if origem == "/proc/self/maps" && (total == 0 || exec == 0) {
		panic("mapa vazio: invariante violada")
	}

	// Filho: cria processo, colhe saida, propaga codigo.
	proc, err := os.StartProcess("/bin/true", []string{"true"}, &os.ProcAttr{
		Files: []*os.File{os.Stdin, os.Stdout, os.Stderr},
	})
	if err != nil {
		panic(err)
	}
	estado, err := proc.Wait()
	if err != nil || !estado.Success() {
		panic("filho /bin/true falhou")
	}
	fmt.Println("filho /bin/true: ok")

	// Falhas provocadas: arquivo ausente e diretorio como arquivo.
	for _, ruim := range []string{"/tmp/08b_nao_existe.bin", "/tmp"} {
		dados, err := os.ReadFile(ruim)
		if err == nil {
			_ = dados
			panic("INESPERADO: " + ruim + " abriu")
		}
		fmt.Printf("falha contida (%v): %s\n", err, ruim)
	}
}
```

Saída esperada (números variam por máquina e SO):

```text
cpus=N gomaxprocs=N pid=PID pagina=4096
getpid: ~ns/chamada (1000000 repeticoes)
antes: heap_mb=~ objetos=~ gcs=~ pausa_total_ms=~
com-bloco-vivo: heap_mb=~8 objetos=~ gcs=~ pausa_total_ms=~
apos-gc: heap_mb=~0.1 objetos=~ gcs=~ pausa_total_ms=~
regioes=N executaveis=M origem=/proc/self/maps (no Linux)
filho /bin/true: ok
falha contida (...): /tmp/08b_nao_existe.bin
falha contida (...): /tmp
```

Leia com o modelo certo: o heap marca ~8 MiB enquanto o bloco está vivo (`com-bloco-vivo`) e cai após o `KeepAlive` + `GC` — o coletor reclama o que nada referencia, e a prova está no número, não na intuição. Fora do Linux, `regioes=0` com origem declarada — lacuna de plataforma, não aprovação presumida. `/bin/true` não existe no Windows: lá, troque por um executável local e anote.

### O que a memória faz aqui?

Os 8 MiB vivem no heap gerenciado: o runtime pede páginas ao SO e o GC decide quando devolver. A vida do bloco termina onde o `KeepAlive` manda — sem ele, o compilador prova que nada usa o bloco e o GC coleta até antes do relatório, e `_ = bloco` não salva ninguém. `runtime.GC()` aqui é instrumento de medição, não otimização: em produção, nunca chame o GC à mão para "otimizar". O mapa mostra faixas virtuais com permissões; nada revela quadros físicos — ver a aula C 10a.

## 5. Erros comuns e implicações defensivas

1. **Presumir CPUs:** quota de contêiner, afinidade e `GOMAXPROCS` manual mudam o paralelismo real. Leia o ambiente.
2. **Confundir heap com RSS:** vazamento fora do heap não aparece em `MemStats`.
3. **Esquecer `Wait`:** filho sem colheita retém estado. Colha todo processo criado.
4. **Ler byte a byte:** cada `Read` curto é uma travessia. Leia em blocos.
5. **Presumir `/proc`:** é Linux. Em outro SO, declare a lacuna e meça por outra via.
6. **Medir sem ambiente:** vDSO, mitigações e carga mudam a régua. Anote máquina, SO e carga.

## 6. Ponte com Python e JavaScript

Python: `os.getpid`, `resource.getrusage` e `subprocess.run` cobrem o mesmo terreno (aula 14a). Node: `process.pid`, `os.cpus()`, `child_process` com `status`. Nos três, a moral é igual: processo é isolamento com contabilidade, e toda travessia ao kernel tem custo mensurável.

## 7. Exercícios obrigatórios

1. Desenhe o caminho de `os.Getpid` até o kernel e de volta, marcando onde o vDSO pode evitar a troca de modo.
2. Varie `GOMAXPROCS` (1, N) num laço paralelo simples e meça: quando mais CPUs não ajudam? Explique com contenção, não com slogan.
3. Provoque `EACCES` de verdade (arquivo sem leitura para seu usuário) e registre o erro exato que Go devolve.
4. Compare `HeapAlloc` após alocar 100 MB e soltar a referência: o que volta ao runtime e o que volta ao SO?

Use [pistas graduais](exercicios/08b_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/08b_solucoes.md) depois de registrar sua própria explicação.

## 8. Ao final, você consegue

Você concluiu a aula se consegue, sem olhar a solução:

- explicar o papel do runtime entre o programa e o kernel;
- medir CPUs, `GOMAXPROCS`, syscall e heap, e listar o que muda cada número;
- interpretar o mapa de memória sem inferir endereços físicos;
- distinguir heap do runtime de RSS e dizer onde procurar cada vazamento;
- tratar arquivo ausente, sem permissão e diretório como respostas normais do SO;
- executar o exemplo com `go vet` limpo e confrontar cada número com o modelo.

**Próxima aula:** ponteiros, memória e escape analysis. As réguas desta aula (heap, páginas, travessias) dizem para onde cada alocação foge.

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

1. Alan Donovan e Brian Kernighan, *The Go Programming Language* (Addison-Wesley, ISBN 978-0134190440); 2. Documentação oficial em [go.dev/doc](https://go.dev/doc/); 3. Referência de pacotes em [pkg.go.dev](https://pkg.go.dev/).
