package main

import (
	"bufio"
	"fmt"
	"os"
	"sort"
	"strconv"
	"strings"
)

type No struct {
	E    int
	Prox *No
}

func SemRepeticoes(L *No) *No {
	seen := make(map[int]bool)
	var result *No
	var tail *No
	for p := L; p != nil; p = p.Prox {
		if !seen[p.E] {
			seen[p.E] = true
			novo := &No{E: p.E, Prox: nil}
			if result == nil {
				result = novo
			} else {
				tail.Prox = novo
			}
			tail = novo
		}
	}
	return result
}

func insere(L **No, e int) {
	novo := &No{E: e, Prox: *L}
	*L = novo
}

func escreve(L *No) {
	var vals []int
	for p := L; p != nil; p = p.Prox {
		vals = append(vals, p.E)
	}
	sort.Ints(vals)
	fmt.Printf("[")
	for i, v := range vals {
		if i > 0 {
		}
		fmt.Printf("%d", v)
	}
	fmt.Printf("]\n")
}

func destroi(L **No) {
	p := *L
	for p != nil {
		pr := p
		p = p.Prox
		_ = pr
	}
	*L = nil
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)
	var L *No
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		n, _ := strconv.Atoi(line)
		if n > -1 {
			for i := 0; i < n; i++ {
				scanner.Scan()
				el := strings.TrimSpace(scanner.Text())
				e, _ := strconv.Atoi(el)
				insere(&L, e)
			}
			Lr := SemRepeticoes(L)
			escreve(Lr)
			destroi(&Lr)
			destroi(&L)
		} else {
			if n == -1 {
				n = 1000
				for i := 0; i < n; i++ {
					if i%2 == 0 {
						insere(&L, 2)
					} else {
						insere(&L, 3)
					}
				}
				Lr := SemRepeticoes(L)
				escreve(Lr)
				destroi(&Lr)
				destroi(&L)
			} else {
				n = 1000
				for i := 0; i < n; i++ {
					insere(&L, i)
				}
				Lr := SemRepeticoes(L)
				escreve(Lr)
				destroi(&Lr)
				destroi(&L)
			}
		}
	}
}
