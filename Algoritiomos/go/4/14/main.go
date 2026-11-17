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

func RemoveMaiores(L **No, k int) {
	// Remove nodes with E > k
	// First handle head
	for *L != nil && (*L).E > k {
		*L = (*L).Prox
	}
	if *L == nil {
		return
	}
	p := *L
	for p.Prox != nil {
		if p.Prox.E > k {
			p.Prox = p.Prox.Prox
		} else {
			p = p.Prox
		}
	}
}

func escreve(L *No) {
	var vals []int
	for p := L; p != nil; p = p.Prox {
		vals = append(vals, p.E)
	}
	sort.Ints(vals)
	fmt.Printf("[")
	for _, v := range vals {
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

func insere(L **No, e int) {
	novo := &No{E: e, Prox: *L}
	*L = novo
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
		maxN, _ := strconv.Atoi(line)
		if maxN != -1 {
			for i := 0; i < maxN; i++ {
				scanner.Scan()
				el := strings.TrimSpace(scanner.Text())
				e, _ := strconv.Atoi(el)
				insere(&L, e)
			}
			scanner.Scan()
			kl := strings.TrimSpace(scanner.Text())
			k, _ := strconv.Atoi(kl)
			RemoveMaiores(&L, k)
			escreve(L)
			destroi(&L)
		} else {
			for t := 1; t <= 20; t++ {
				L = nil
				maxN = 50000
				for i := 0; i < maxN; i++ {
					if i%2 == 0 {
						insere(&L, 2)
					} else {
						insere(&L, 3)
					}
				}
				RemoveMaiores(&L, 2)
				if t == 1 {
					escreve(L)
				}
				destroi(&L)
			}
		}
	}
}
