package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

type Parentese struct {
	desc byte // '(' or ')'
	tipo int
}

func VerificaParentizacao(E []Parentese, n int) int {
	pilha := []int{}
	for i := 0; i < n; i++ {
		if E[i].desc == '(' {
			pilha = append(pilha, E[i].tipo)
		} else {
			if len(pilha) == 0 {
				return 0
			}
			top := pilha[len(pilha)-1]
			pilha = pilha[:len(pilha)-1]
			if top != E[i].tipo {
				return 0
			}
		}
	}
	if len(pilha) == 0 {
		return 1
	}
	return 0
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 10*1024*1024), 10*1024*1024)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		n, _ := strconv.Atoi(line)
		if n >= 0 {
			scanner.Scan()
			Ec := strings.TrimSpace(scanner.Text())
			E := make([]Parentese, n)
			j := 0
			for i := 0; i < n; i++ {
				E[i].desc = Ec[j]
				j++
				z := j
				for j < len(Ec) && Ec[j] != '(' && Ec[j] != ')' {
					j++
				}
				E[i].tipo, _ = strconv.Atoi(Ec[z:j])
			}
			fmt.Println(VerificaParentizacao(E, n))
		} else {
			nlin := n
			n = 1000000
			E := make([]Parentese, n)
			if nlin == -1 {
				for i := 0; i < n/2; i++ {
					E[i].desc = '('
					E[i].tipo = 1
				}
				for i := n / 2; i < n; i++ {
					E[i].desc = ')'
					E[i].tipo = 1
				}
			} else if nlin == -2 {
				for i := 0; i < n/2; i++ {
					E[i].desc = '('
					E[i].tipo = i + 1
				}
				for i := n / 2; i < n; i++ {
					E[i].desc = ')'
					E[i].tipo = n - (i + 1) + 1
				}
			} else if nlin == -3 {
				for i := 0; i < n; i += 2 {
					E[i].desc = '('
					E[i].tipo = i + 1
					E[i+1].desc = ')'
					E[i+1].tipo = i + 1
				}
			} else if nlin == -4 {
				for i := 0; i < n/2-1; i++ {
					E[i].desc = '('
					E[i].tipo = 1
				}
				for i := n/2 - 1; i < n; i++ {
					E[i].desc = ')'
					E[i].tipo = 1
				}
			} else if nlin == -5 {
				for i := 0; i < n/2+1; i++ {
					E[i].desc = '('
					E[i].tipo = 1
				}
				for i := n/2 + 1; i < n; i++ {
					E[i].desc = ')'
					E[i].tipo = 1
				}
			}
			fmt.Println(VerificaParentizacao(E, n))
		}
	}
}
