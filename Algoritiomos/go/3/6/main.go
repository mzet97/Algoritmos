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

func Tamanho(P *Pilha) int {
	return len(P.dados)
}

func ManobrarVagoes(n int, V []int) {
	// Input queue: n,n-1,...,2,1 (front=1, back=n)
	// V[0]=last wagon desired, V[n-1]=first wagon desired
	// We need output: V[n-1] is first out, V[0] is last out
	entrada := &Pilha{}
	Constroi(entrada)
	manobra := &Pilha{}
	Constroi(manobra)

	// Fill entrada with n,n-1,...,2,1 (top=1, bottom=n)
	for i := 1; i <= n; i++ {
		Empilha(entrada, i)
	}

	// We want to output in order: V[n-1], V[n-2], ..., V[0]
	// (V[n-1] is first wagon out, V[0] is last wagon out)
	idx := n - 1
	first := true
	for idx >= 0 {
		// Try to move from manobra to output
		if Tamanho(manobra) > 0 && Topo(manobra) == V[idx] {
			if !first {
				fmt.Printf(",")
			}
			fmt.Printf("M=>S")
			Desempilha(manobra)
			idx--
			first = false
			continue
		}
		// Move from entrada to manobra
		if Tamanho(entrada) > 0 {
			if !first {
				fmt.Printf(",")
			}
			fmt.Printf("E=>M")
			Empilha(manobra, Desempilha(entrada))
			first = false
			continue
		}
		// Try moving from manobra back to entrada (swap)
		if Tamanho(manobra) > 0 {
			if !first {
				fmt.Printf(",")
			}
			fmt.Printf("M=>E")
			Empilha(entrada, Desempilha(manobra))
			first = false
			continue
		}
		break
	}
	fmt.Println()
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		n, _ := strconv.Atoi(line)
		if n != -1 {
			scanner.Scan()
			dataLine := strings.TrimSpace(scanner.Text())
			parts := strings.Fields(dataLine)
			V := make([]int, n)
			for i := 0; i < n; i++ {
				V[i], _ = strconv.Atoi(parts[i])
			}
			ManobrarVagoes(n, V)
		} else {
			n = 1000
			V := make([]int, n)
			M := n
			m := 1
			for i := n - 1; i >= 0; i -= 2 {
				V[i] = M
				V[i-1] = m
				M--
				m++
			}
			ManobrarVagoes(n, V)
		}
	}
}
