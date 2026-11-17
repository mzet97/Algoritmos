package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

type Fila struct {
	dados []int
	front int
}

func Constroi(F *Fila) {
	F.dados = nil
	F.front = 0
}

func Enfileira(F *Fila, x int) {
	F.dados = append(F.dados, x)
}

func Desenfileira(F *Fila) int {
	x := F.dados[F.front]
	F.front++
	return x
}

func Tamanho(F *Fila) int {
	return len(F.dados) - F.front
}

func LerEscreverKUltimos(k int, scanner *bufio.Scanner) {
	F := Fila{}
	Constroi(&F)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		parts := strings.Fields(line)
		for _, p := range parts {
			x, _ := strconv.Atoi(p)
			if x == 0 {
				for Tamanho(&F) > 0 {
					fmt.Printf("%d ", Desenfileira(&F))
				}
				fmt.Println()
				return
			}
			Enfileira(&F, x)
			if Tamanho(&F) > k {
				Desenfileira(&F)
			}
		}
	}
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		k, _ := strconv.Atoi(line)
		LerEscreverKUltimos(k, scanner)
	}
}
