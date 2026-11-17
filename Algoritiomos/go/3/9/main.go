package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func SomaSequenciaOperacoes(S []int, n int) int {
	pilha := []int{}
	for i := 0; i < n; i++ {
		if S[i] == 0 {
			if len(pilha) > 0 {
				pilha = pilha[:len(pilha)-1]
			}
		} else {
			pilha = append(pilha, S[i])
		}
	}
	soma := 0
	for _, v := range pilha {
		soma += v
	}
	return soma
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
			parts := strings.Fields(strings.TrimSpace(scanner.Text()))
			S := make([]int, n)
			for i := 0; i < n; i++ {
				S[i], _ = strconv.Atoi(parts[i])
			}
			fmt.Println(SomaSequenciaOperacoes(S, n))
		} else {
			n = 1000000
			S := make([]int, n)
			for i := 0; i < n/2; i++ {
				S[i] = 1
				S[n/2+i] = 0
			}
			fmt.Println(SomaSequenciaOperacoes(S, n))
		}
	}
}
