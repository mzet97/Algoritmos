package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

type Pilha struct {
	dados []int
}

func Constroi(P *Pilha) {
	P.dados = nil
}

func Destroi(P *Pilha) {
	P.dados = nil
}

func Empilha(P *Pilha, x int) {
	P.dados = append(P.dados, x)
}

func Desempilha(P *Pilha) int {
	n := len(P.dados) - 1
	x := P.dados[n]
	P.dados = P.dados[:n]
	return x
}

func Tamanho(P *Pilha) int {
	return len(P.dados)
}

func PilhasIdenticas(p1 *Pilha, p2 *Pilha) int {
	if Tamanho(p1) != Tamanho(p2) {
		return 0
	}
	aux1 := []int{}
	aux2 := []int{}
	n := Tamanho(p1)
	iguais := 1
	for i := 0; i < n; i++ {
		v1 := Desempilha(p1)
		v2 := Desempilha(p2)
		aux1 = append(aux1, v1)
		aux2 = append(aux2, v2)
		if v1 != v2 {
			iguais = 0
		}
	}
	for j := len(aux1) - 1; j >= 0; j-- {
		Empilha(p1, aux1[j])
	}
	for j := len(aux2) - 1; j >= 0; j-- {
		Empilha(p2, aux2[j])
	}
	return iguais
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	p1 := &Pilha{}
	p2 := &Pilha{}
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		n, _ := strconv.Atoi(line)
		if n != -1 {
			Constroi(p1)
			Constroi(p2)
			scanner.Scan()
			parts := strings.Fields(strings.TrimSpace(scanner.Text()))
			for i := 0; i < n; i++ {
				e, _ := strconv.Atoi(parts[i])
				Empilha(p1, e)
			}
			scanner.Scan()
			n2, _ := strconv.Atoi(strings.TrimSpace(scanner.Text()))
			scanner.Scan()
			parts = strings.Fields(strings.TrimSpace(scanner.Text()))
			for i := 0; i < n2; i++ {
				e, _ := strconv.Atoi(parts[i])
				Empilha(p2, e)
			}
			fmt.Println(PilhasIdenticas(p1, p2))
			Destroi(p1)
			Destroi(p2)
		} else {
			Constroi(p1)
			Constroi(p2)
			n = 1000000
			for i := 0; i < n; i++ {
				Empilha(p1, i+1)
				Empilha(p2, i+1)
			}
			fmt.Println(PilhasIdenticas(p1, p2))
			Destroi(p1)
			Destroi(p2)
		}
	}
}
