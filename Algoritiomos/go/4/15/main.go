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
	Prox *No
}

func Inverte(L *No) *No {
	var prev *No
	curr := L
	for curr != nil {
		next := curr.Prox
		curr.Prox = prev
		prev = curr
		curr = next
	}
	return prev
}

func insere(L **No, e int) {
	novo := &No{E: e, Prox: *L}
	*L = novo
}

func escreve(L *No) {
	fmt.Printf("[ ")
	for p := L; p != nil; p = p.Prox {
		fmt.Printf("%d ", p.E)
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
		if n != -1 {
			for i := 0; i < n; i++ {
				scanner.Scan()
				el := strings.TrimSpace(scanner.Text())
				e, _ := strconv.Atoi(el)
				insere(&L, e)
			}
			Lr := Inverte(L)
			escreve(Lr)
			destroi(&Lr)
			destroi(&L)
		} else {
			n = 1000
			for i := 0; i < n; i++ {
				insere(&L, i%100)
			}
			for t := 1; t <= 1000; t++ {
				Lr := Inverte(L)
				destroi(&Lr)
			}
			Lr := Inverte(L)
			escreve(Lr)
			destroi(&Lr)
			destroi(&L)
		}
	}
}
