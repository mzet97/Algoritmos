// Aula 28c Go — Multino simulado: a triade em dois processos.
//
// Dois trabalhadores (o mesmo binario, argv "trabalhador") atendem por
// TCP em 127.0.0.1 com porta efemera; o coordenador divide a triade
// `a = b + s*c` ao meio, cada metade e calculada num processo
// distinto e o checksum combinado e confrontado com a referencia
// local. Igualdade de checksum e assert; razao de tempo e medida
// publicada — nesta maquina de 1 no, o overhead do loopback aparece e
// tambem e resultado.
//
// Validar: go vet teoria/go/exemplos/28c_multino.go && go run -race teoria/go/exemplos/28c_multino.go
package main

import (
	"bufio"
	"fmt"
	"net"
	"os"
	"os/exec"
	"strconv"
	"strings"
	"time"
)

const total = 4 << 20 // 4 MiB em f32
const ganho = 2.5

func gera(v []float32, base float64) {
	for i := range v {
		v[i] = float32(base + float64(i)*0.5)
	}
}

func triade(a, b, c []float32) {
	for i := range a {
		a[i] = b[i] + ganho*c[i]
	}
}

func soma(v []float32) float64 {
	var t float64
	for _, x := range v {
		t += float64(x)
	}
	return t
}

// Trabalhador: disca para o coordenador, recebe "METADE ini fim", calcula
// sua metade a partir da mesma semente deterministica e devolve o
// checksum com o tempo. Nenhum vetor viaja: dado derivado, nao movido.
func trabalhador(endereco string) {
	conn, err := net.DialTimeout("tcp", endereco, 5*time.Second)
	if err != nil {
		panic(err)
	}
	defer conn.Close()
	conn.SetDeadline(time.Now().Add(60 * time.Second))
	leitor := bufio.NewReader(conn)
	linha, err := leitor.ReadString('\n')
	if err != nil {
		panic(err)
	}
	partes := strings.Fields(linha)
	if len(partes) != 3 || partes[0] != "METADE" {
		panic("protocolo: " + linha)
	}
	ini, _ := strconv.Atoi(partes[1])
	fim, _ := strconv.Atoi(partes[2])
	b := make([]float32, total)
	c := make([]float32, total)
	gera(b, 1.0)
	gera(c, 3.0)
	a := make([]float32, fim-ini)
	t0 := time.Now()
	for i := range a {
		a[i] = b[ini+i] + ganho*c[ini+i]
	}
	dt := time.Since(t0)
	fmt.Fprintf(conn, "OK %.17g %d\n", soma(a), dt.Nanoseconds())
}

func serveMetade(ouvinte net.Listener, ini, fim int) (float64, time.Duration, error) {
	conn, err := ouvinte.Accept()
	if err != nil {
		return 0, 0, err
	}
	defer conn.Close()
	conn.SetDeadline(time.Now().Add(60 * time.Second))
	fmt.Fprintf(conn, "METADE %d %d\n", ini, fim)
	leitor := bufio.NewReader(conn)
	linha, err := leitor.ReadString('\n')
	if err != nil {
		return 0, 0, err
	}
	partes := strings.Fields(linha)
	if len(partes) != 3 || partes[0] != "OK" {
		return 0, 0, fmt.Errorf("protocolo: %s", linha)
	}
	s, _ := strconv.ParseFloat(partes[1], 64)
	ns, _ := strconv.ParseInt(partes[2], 10, 64)
	return s, time.Duration(ns), nil
}

func main() {
	if len(os.Args) > 1 && os.Args[1] == "trabalhador" {
		trabalhador(os.Args[2])
		return
	}
	ouvinte, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		panic(err)
	}
	defer ouvinte.Close()
	endereco := ouvinte.Addr().String()

	exe, err := os.Executable()
	if err != nil {
		panic(err)
	}
	metades := [][2]int{{0, total / 2}, {total / 2, total}}
	type resultado struct {
		soma float64
		dt   time.Duration
		err  error
	}
	res := make(chan resultado, 2)
	go func() {
		s, dt, err := serveMetade(ouvinte, metades[0][0], metades[0][1])
		res <- resultado{s, dt, err}
	}()
	go func() {
		s, dt, err := serveMetade(ouvinte, metades[1][0], metades[1][1])
		res <- resultado{s, dt, err}
	}()
	for i := 0; i < 2; i++ {
		cmd := exec.Command(exe, "trabalhador", endereco)
		cmd.Stdout = os.Stderr
		cmd.Stderr = os.Stderr
		if err := cmd.Start(); err != nil {
			panic(err)
		}
		defer cmd.Wait()
	}
	r1 := <-res
	r2 := <-res
	if r1.err != nil || r2.err != nil {
		panic(fmt.Sprintf("trabalhador falhou: %v %v", r1.err, r2.err))
	}

	// Referencia local: mesma conta, um processo so.
	b := make([]float32, total)
	c := make([]float32, total)
	a := make([]float32, total)
	gera(b, 1.0)
	gera(c, 3.0)
	t0 := time.Now()
	triade(a, b, c)
	tLocal := time.Since(t0)
	ref := soma(a)

	combinada := r1.soma + r2.soma
	if combinada != ref {
		panic(fmt.Sprintf("checksum divergiu: %g vs %g", combinada, ref))
	}
	tRemoto := r1.dt
	if r2.dt > tRemoto {
		tRemoto = r2.dt
	}
	fmt.Printf("referencia local: %.3f GB/s em %v\n", float64(3*total*4)/1e9/tLocal.Seconds(), tLocal)
	fmt.Printf("duas metades remotas: pior em %v (loopback, 1 no)\n", tRemoto)
	fmt.Printf("razao remoto/local: %.2fx (medida, nao veredito)\n", float64(tRemoto)/float64(tLocal))
	fmt.Println("28c ok: checksum combinado igual a referencia, protocolo com deadline, multino simulado declarado")
}
