package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

type No struct {
	E    int
	Esq  *No
	Dir  *No
}

func CriarArvoreAlturaMinima(x []int, ini, fim int) *No {
	if ini > fim {
		return nil
	}
	mid := (ini + fim) / 2
	T := &No{E: x[mid]}
	T.Esq = CriarArvoreAlturaMinima(x, ini, mid-1)
	T.Dir = CriarArvoreAlturaMinima(x, mid+1, fim)
	return T
}

func destroi(T **No) {
	if *T != nil {
		destroi(&(*T).Esq)
		destroi(&(*T).Dir)
		*T = nil
	}
}

func escrever(T *No) {
	if T != nil {
		fmt.Printf("(%d ", T.E)
		fmt.Printf("(")
		escrever(T.Esq)
		fmt.Printf(")")
		fmt.Printf("(")
		escrever(T.Dir)
		fmt.Printf("))")
	}
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	e := make([]int, 1000000)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		e[0], _ = strconv.Atoi(line)
		n := 0
		if e[0] > -1 {
			for e[n] != -1 {
				n++
				scanner.Scan()
				e[n], _ = strconv.Atoi(strings.TrimSpace(scanner.Text()))
			}
		} else {
			for n = 0; n < 100000; n++ {
				e[n] = n + 1
			}
			e[n] = -1
		}
		T := CriarArvoreAlturaMinima(e, 0, n-1)
		escrever(T)
		fmt.Println()
		destroi(&T)
	}
}
