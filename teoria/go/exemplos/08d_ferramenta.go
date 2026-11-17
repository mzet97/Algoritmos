// Aula 08d — ferramenta de observacao do SO em Go: goroutines, memoria
// do runtime, threads e faltas de pagina do proprio processo (Linux).
package main

import (
	"fmt"
	"os"
	"runtime"
	"strconv"
	"strings"
	"time"
)

func falhar(msg string) {
	fmt.Fprintln(os.Stderr, "FALHA:", msg)
	os.Exit(1)
}

// /proc/self/stat: campos apos o comm entre parenteses. minflt e o
// campo 10, majflt o 12 (contagem comeca no campo 3 = state).
func faltasPagina() (minflt, majflt uint64, ok bool) {
	dados, err := os.ReadFile("/proc/self/stat")
	if err != nil {
		return 0, 0, false
	}
	s := string(dados)
	fecha := strings.LastIndex(s, ")")
	if fecha < 0 {
		return 0, 0, false
	}
	campos := strings.Fields(s[fecha+1:])
	// campos[0] = state (campo 3); minflt = campo 10 -> indice 7.
	if len(campos) < 10 {
		return 0, 0, false
	}
	minflt, err1 := strconv.ParseUint(campos[7], 10, 64)
	majflt, err2 := strconv.ParseUint(campos[9], 10, 64)
	return minflt, majflt, err1 == nil && err2 == nil
}

func threadsSO() int {
	entradas, err := os.ReadDir("/proc/self/task")
	if err != nil {
		return -1
	}
	return len(entradas)
}

func main() {
	fmt.Printf("goroutines(parado)=%d gomaxprocs=%d threads_so=%d\n",
		runtime.NumGoroutine(), runtime.GOMAXPROCS(0), threadsSO())

	minAntes, majAntes, ok := faltasPagina()
	if !ok {
		falhar("/proc/self/stat indisponivel (ferramenta e Linux)")
	}

	// Toca 32 MiB pagina a pagina: minflt deve crescer, majflt nao
	// (nada vem do disco; pagina anonima nasce por falta menor).
	bloco := make([]byte, 32<<20)
	for i := 0; i < len(bloco); i += 4096 {
		bloco[i] = 1
	}
	runtime.KeepAlive(bloco)

	minDepois, majDepois, _ := faltasPagina()
	var m runtime.MemStats
	runtime.ReadMemStats(&m)
	fmt.Printf("page_faults: min %d->%d (+%d) maj %d->%d\n",
		minAntes, minDepois, minDepois-minAntes, majAntes, majDepois)
	fmt.Printf("heap_alloc=%d MiB heap_sys=%d MiB gc_cycles=%d\n",
		m.HeapAlloc>>20, m.HeapSys>>20, m.NumGC)
	if minDepois <= minAntes {
		falhar("minflt deveria crescer ao tocar 32 MiB novos")
	}
	if majDepois != majAntes {
		falhar("majflt nao deveria crescer para paginas anonimas novas")
	}

	// Goroutines sobem e descem de forma observavel.
	const trabalhadores = 32
	feito := make(chan struct{})
	for i := 0; i < trabalhadores; i++ {
		go func() {
			time.Sleep(50 * time.Millisecond)
			feito <- struct{}{}
		}()
	}
	fmt.Printf("goroutines(com trabalhadores)=%d\n", runtime.NumGoroutine())
	if runtime.NumGoroutine() < trabalhadores {
		falhar("goroutines dos trabalhadores nao observadas")
	}
	for i := 0; i < trabalhadores; i++ {
		<-feito
	}
	fmt.Println("08d ok: ferramenta observa goroutines, memoria, threads e page faults")
}
