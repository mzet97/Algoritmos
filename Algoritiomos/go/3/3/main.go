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

func Ordena(P *Pilha) {
	Q := Pilha{}
	Constroi(&Q)
	for Tamanho(P) > 0 {
		tmp := Desempilha(P)
		for Tamanho(&Q) > 0 && Topo(&Q) > tmp {
			Empilha(P, Desempilha(&Q))
		}
		Empilha(&Q, tmp)
	}
	for Tamanho(&Q) > 0 {
		Empilha(P, Desempilha(&Q))
	}
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	P := Pilha{}
	Constroi(&P)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		n, _ := strconv.Atoi(line)
		if n >= 0 {
			scanner.Scan()
			dataLine := strings.TrimSpace(scanner.Text())
			parts := strings.Fields(dataLine)
			for i := 0; i < n; i++ {
				e, _ := strconv.Atoi(parts[i])
				Empilha(&P, e)
			}
			Ordena(&P)
			for Tamanho(&P) > 0 {
				fmt.Printf("%d ", Desempilha(&P))
			}
			fmt.Println()
		} else {
			if n == -1 {
				for i := 1; i <= 1000; i++ {
					Empilha(&P, i+(1001)*(i%2))
				}
			} else {
				for i := 1; i <= 1000; i++ {
					v := i + (1001)
					if i%5 == 0 {
						v = i
					}
					Empilha(&P, v)
				}
			}
			Ordena(&P)
			for Tamanho(&P) > 0 {
				fmt.Printf("%d ", Desempilha(&P))
			}
			fmt.Println()
		}
	}
}
