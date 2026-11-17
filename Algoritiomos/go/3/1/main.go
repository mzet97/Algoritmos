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

func ConstroiPilha(P *Pilha) {
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

func Topo(P *Pilha) int {
	return P.dados[len(P.dados)-1]
}

func TamanhoPilha(P *Pilha) int {
	return len(P.dados)
}

type Fila struct {
	P1 Pilha
	P2 Pilha
}

func ConstroiFila(F *Fila) {
	ConstroiPilha(&F.P1)
	ConstroiPilha(&F.P2)
}

func Enfileira(F *Fila, x int) {
	Empilha(&F.P1, x)
}

func Desenfileira(F *Fila) int {
	if TamanhoPilha(&F.P2) == 0 {
		for TamanhoPilha(&F.P1) > 0 {
			Empilha(&F.P2, Desempilha(&F.P1))
		}
	}
	return Desempilha(&F.P2)
}

func Proximo(F *Fila) int {
	if TamanhoPilha(&F.P2) == 0 {
		for TamanhoPilha(&F.P1) > 0 {
			Empilha(&F.P2, Desempilha(&F.P1))
		}
	}
	return Topo(&F.P2)
}

func TamanhoFila(F *Fila) int {
	return TamanhoPilha(&F.P1) + TamanhoPilha(&F.P2)
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	F := Fila{}
	ConstroiFila(&F)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		parts := strings.Fields(line)
		op := parts[0]
		if op == "i" {
			e, _ := strconv.Atoi(parts[1])
			Enfileira(&F, e)
		} else if op == "r" {
			fmt.Printf("%d\n", Desenfileira(&F))
		} else if op == "p" {
			fmt.Printf("%d\n", Proximo(&F))
		} else if op == "n" {
			fmt.Printf("%d\n", TamanhoFila(&F))
		} else {
			for i := 1; i <= 1000; i++ {
				Enfileira(&F, i)
			}
			for TamanhoFila(&F) > 0 {
				fmt.Printf("%d ", TamanhoFila(&F))
				fmt.Printf("%d ", Proximo(&F))
				fmt.Printf("%d ", Desenfileira(&F))
			}
			fmt.Println()
		}
	}
}
