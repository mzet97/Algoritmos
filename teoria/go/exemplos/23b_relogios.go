// Aula 23b (Go) — relogios logicos e particao bilateral sobre a 23a:
// Lamport ordena, vetor detecta concorrencia, anti-entropia com ordem
// total declarada reconcilia nomeando a escrita descartada.
package main

import (
	"fmt"
	"os"
	"sort"
)

type evento struct {
	lamport int
	vetor   [2]int
	autor   int
	seq     int
	chave   string
	valor   int
}

type noSim struct {
	lamport int
	vetor   [2]int
	log     []evento
	estado  map[string]int
}

func falhar(msg string) {
	fmt.Fprintln(os.Stderr, "FALHA:", msg)
	os.Exit(1)
}

func (n *noSim) escrever(autor, seq int, chave string, valor int) evento {
	n.lamport++
	n.vetor[autor]++
	e := evento{lamport: n.lamport, vetor: n.vetor, autor: autor, seq: seq, chave: chave, valor: valor}
	n.log = append(n.log, e)
	n.aplicar(e)
	return e
}

func (n *noSim) aplicar(e evento) {
	if n.estado == nil {
		n.estado = map[string]int{}
	}
	n.estado[e.chave] = e.valor
}

func (n *noSim) receber(e evento) bool {
	for _, x := range n.log {
		if x.autor == e.autor && x.seq == e.seq {
			return false
		}
	}
	if e.lamport > n.lamport {
		n.lamport = e.lamport
	}
	n.lamport++
	for i := range n.vetor {
		if e.vetor[i] > n.vetor[i] {
			n.vetor[i] = e.vetor[i]
		}
	}
	n.log = append(n.log, e)
	return true
}

func antes(a, b evento) bool {
	menor := false
	for i := 0; i < 2; i++ {
		if a.vetor[i] > b.vetor[i] {
			return false
		}
		if a.vetor[i] < b.vetor[i] {
			menor = true
		}
	}
	return menor
}

func concorrentes(a, b evento) bool {
	return !antes(a, b) && !antes(b, a) && !(a.autor == b.autor && a.seq == b.seq)
}

func sincronizar(a, b *noSim) {
	la := append([]evento(nil), a.log...)
	lb := append([]evento(nil), b.log...)
	for _, e := range lb {
		a.receber(e)
	}
	for _, e := range la {
		b.receber(e)
	}
	for _, n := range []*noSim{a, b} {
		sort.SliceStable(n.log, func(i, j int) bool {
			x, y := n.log[i], n.log[j]
			if x.lamport != y.lamport {
				return x.lamport < y.lamport
			}
			if x.autor != y.autor {
				return x.autor < y.autor
			}
			return x.seq < y.seq
		})
		// dedup apos ordenar
		unicos := n.log[:0]
		vistos := map[[2]int]bool{}
		for _, e := range n.log {
			k := [2]int{e.autor, e.seq}
			if !vistos[k] {
				vistos[k] = true
				unicos = append(unicos, e)
			}
		}
		n.log = unicos
		n.estado = map[string]int{}
		for _, e := range n.log {
			n.aplicar(e)
		}
	}
}

func main() {
	a, b := &noSim{}, &noSim{}
	e1 := a.escrever(0, 1, "x", 10)
	if !b.receber(e1) || b.receber(e1) {
		falhar("duplicata deveria ser reconhecida")
	}
	e2 := b.escrever(1, 1, "y", 20)
	if !antes(e1, e2) || concorrentes(e1, e2) {
		falhar("causalidade A->B nao detectada")
	}
	fmt.Printf("lamport: e1=%d e2=%d causalidade preservada\n", e1.lamport, e2.lamport)

	a2, b2 := &noSim{}, &noSim{}
	p1 := a2.escrever(0, 1, "k", 111)
	p2 := b2.escrever(1, 1, "k", 222)
	if !concorrentes(p1, p2) {
		falhar("particao deveria gerar eventos concorrentes")
	}
	if a2.estado["k"] != 111 || b2.estado["k"] != 222 {
		falhar("divergencia bilateral nao reproduzida")
	}
	fmt.Println("particao: A ve k=111, B ve k=222, concorrencia detectada pelo vetor")

	sincronizar(a2, b2)
	if a2.estado["k"] != 222 || b2.estado["k"] != 222 {
		falhar("ordem total (lamport,autor): o ultimo aplicado vence (222)")
	}
	fmt.Println("reconciliacao: ambos k=222; k=111 descartado pela regra publicada")

	e3 := b2.escrever(1, 2, "k", 333)
	if !antes(p1, e3) || !antes(p2, e3) {
		falhar("escrita pos-sincronizacao deveria ser causalmente posterior")
	}
	sincronizar(a2, b2)
	if a2.estado["k"] != 333 || b2.estado["k"] != 333 {
		falhar("escrita causal deveria vencer nos dois")
	}
	fmt.Println("pos-sincronizacao: k=333 vence por causalidade, sem desempate")
	fmt.Println("23b ok: Lamport, vetor, particao e reconciliacao com perda declarada")
}
