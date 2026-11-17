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
	Q    int
	Esq  *No
	Dir  *No
}

func Insere(T **No, x int) {
	if *T == nil {
		*T = &No{E: x, Q: 1}
	} else if (*T).E > x {
		Insere(&(*T).Esq, x)
		(*T).Q++
	} else if (*T).E < x {
		Insere(&(*T).Dir, x)
		(*T).Q++
	}
	// if equal, do nothing (no duplicates counted)
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
		fmt.Printf("(Q=%d,E=%d ", T.Q, T.E)
		fmt.Printf("(")
		escrever(T.Esq)
		fmt.Printf(")")
		fmt.Printf("(")
		escrever(T.Dir)
		fmt.Printf("))")
	}
}

func insereBal(T **No, i, j int) {
	if i <= j {
		m := (i + j) / 2
		Insere(T, m)
		insereBal(T, i, m-1)
		insereBal(T, m+1, j)
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
		n, _ := strconv.Atoi(line)
		var T *No
		if n > 0 {
			for i := 0; i < n; i++ {
				scanner.Scan()
				x, _ := strconv.Atoi(strings.TrimSpace(scanner.Text()))
				Insere(&T, x)
			}
		} else {
			insereBal(&T, 1, 50000)
			insereBal(&T, 1, 50000)
		}
		escrever(T)
		fmt.Println()
		destroi(&T)
	}
}
