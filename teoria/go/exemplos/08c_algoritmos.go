// Aula 08c — duas solucoes medidas em Go: BST e ordenacao contra a std.
package main

import (
	"fmt"
	"os"
	"sort"
	"time"
)

type estado uint64

func (e *estado) prox() uint32 {
	*e = *e*6364136223846793005 + 1
	return uint32(*e >> 33)
}

type no struct {
	chave       int
	esq, dir    *no
}

func inserir(raiz *no, chave int) (*no, bool) {
	if raiz == nil {
		return &no{chave: chave}, true
	}
	switch {
	case chave == raiz.chave:
		return raiz, false
	case chave < raiz.chave:
		var ok bool
		raiz.esq, ok = inserir(raiz.esq, chave)
		return raiz, ok
	default:
		var ok bool
		raiz.dir, ok = inserir(raiz.dir, chave)
		return raiz, ok
	}
}

func emOrdem(raiz *no, saida *[]int) {
	if raiz == nil {
		return
	}
	emOrdem(raiz.esq, saida)
	*saida = append(*saida, raiz.chave)
	emOrdem(raiz.dir, saida)
}

func altura(raiz *no) int {
	if raiz == nil {
		return 0
	}
	e, d := altura(raiz.esq), altura(raiz.dir)
	if e > d {
		return e + 1
	}
	return d + 1
}

func falhar(msg string) {
	fmt.Fprintln(os.Stderr, "FALHA:", msg)
	os.Exit(1)
}

func main() {
	est := estado(7)
	for teste := 0; teste < 300; teste++ {
		n := int(est.prox() % 60)
		valores := make([]int, n)
		var arv *no
		distintas := 0
		for i := range valores {
			valores[i] = int(est.prox()%40) - 20
			var ok bool
			arv, ok = inserir(arv, valores[i])
			if ok {
				distintas++
			}
		}
		ordenado := append([]int(nil), valores...)
		sort.Ints(ordenado)
		unicos := ordenado[:0]
		for i, v := range ordenado {
			if i == 0 || v != ordenado[i-1] {
				unicos = append(unicos, v)
			}
		}
		var saida []int
		emOrdem(arv, &saida)
		if len(saida) != distintas || len(saida) != len(unicos) {
			falhar("tamanhos divergem")
		}
		for i := range saida {
			if saida[i] != unicos[i] {
				falhar("em-ordem diverge do oraculo")
			}
		}
	}
	fmt.Println("propriedade ok: BST em-ordem == sort.Ints+dedup (300 casos)")

	var lista *no
	for i := 0; i < 1000; i++ {
		lista, _ = inserir(lista, i)
	}
	if altura(lista) != 1000 {
		falhar("degenerada deveria ter altura 1000")
	}
	perm := make([]int, 1000)
	for i := range perm {
		perm[i] = i
	}
	for i := 999; i > 0; i-- {
		j := int(est.prox() % uint32(i+1))
		perm[i], perm[j] = perm[j], perm[i]
	}
	var emb *no
	for _, v := range perm {
		emb, _ = inserir(emb, v)
	}
	h := altura(emb)
	fmt.Printf("altura ordenada=1000 embaralhada=%d\n", h)
	if h >= 100 {
		falhar("embaralhada alta demais")
	}

	// Medicao: sort da std contra insercao quadratica em dois tamanhos.
	for _, n := range []int{4000, 16000} {
		base := make([]int, n)
		for i := range base {
			base[i] = int(est.prox())
		}
		a := append([]int(nil), base...)
		t0 := time.Now()
		sort.Ints(a)
		tStd := time.Since(t0)
		b := append([]int(nil), base[:2000]...)
		t0 = time.Now()
		for i := 1; i < len(b); i++ {
			x := b[i]
			j := i
			for j > 0 && b[j-1] > x {
				b[j] = b[j-1]
				j--
			}
			b[j] = x
		}
		tIns := time.Since(t0)
		fmt.Printf("n=%d std=%v insercao(2000)=%v\n", n, tStd, tIns)
	}
	fmt.Println("08c ok: duas solucoes, invariante provada e degeneracao medida")
}
