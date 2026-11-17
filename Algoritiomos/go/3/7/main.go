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

func Destroi(F *Fila) {
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

type FilaPair struct {
	val int
	idx int
}

func FilasIdenticas(f1 *Fila, f2 *Fila) int {
	if Tamanho(f1) != Tamanho(f2) {
		return 0
	}
	aux1 := []FilaPair{}
	aux2 := []FilaPair{}
	n := Tamanho(f1)
	for i := 0; i < n; i++ {
		v1 := Desenfileira(f1)
		v2 := Desenfileira(f2)
		if v1 != v2 {
			// restore
			for _, p := range aux1 {
				Enfileira(f1, p.val)
			}
			Enfileira(f1, v1)
			for _, p := range aux2 {
				Enfileira(f2, p.val)
			}
			Enfileira(f2, v2)
			return 0
		}
		aux1 = append(aux1, FilaPair{v1, i})
		aux2 = append(aux2, FilaPair{v2, i})
	}
	for _, p := range aux1 {
		Enfileira(f1, p.val)
	}
	for _, p := range aux2 {
		Enfileira(f2, p.val)
	}
	return 1
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	f1 := &Fila{}
	f2 := &Fila{}
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		n, _ := strconv.Atoi(line)
		if n != -1 {
			Constroi(f1)
			Constroi(f2)
			scanner.Scan()
			parts := strings.Fields(strings.TrimSpace(scanner.Text()))
			for i := 0; i < n; i++ {
				e, _ := strconv.Atoi(parts[i])
				Enfileira(f1, e)
			}
			scanner.Scan()
			n2, _ := strconv.Atoi(strings.TrimSpace(scanner.Text()))
			scanner.Scan()
			parts = strings.Fields(strings.TrimSpace(scanner.Text()))
			for i := 0; i < n2; i++ {
				e, _ := strconv.Atoi(parts[i])
				Enfileira(f2, e)
			}
			fmt.Println(FilasIdenticas(f1, f2))
			Destroi(f1)
			Destroi(f2)
		} else {
			Constroi(f1)
			Constroi(f2)
			n = 1000000
			for i := 0; i < n; i++ {
				Enfileira(f1, i+1)
				Enfileira(f2, i+1)
			}
			fmt.Println(FilasIdenticas(f1, f2))
			Destroi(f1)
			Destroi(f2)
		}
	}
}
