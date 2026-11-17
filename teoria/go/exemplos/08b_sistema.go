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
