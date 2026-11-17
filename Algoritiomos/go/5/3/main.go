package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

type No struct {
	Op   byte // '+' '-' '*' '/' or '@' for number
	E    int
	Esq  *No
	Dir  *No
}

func ExpressaoParaArvore(C string, ini, fim int) *No {
	if ini > fim {
		return nil
	}
	// Find the top-level operator
	// The expression is parenthesized: (left op right) or a digit
	if C[ini] == '(' {
		// Find matching close paren
		depth := 0
		splitPos := -1
		for i := ini; i <= fim; i++ {
			if C[i] == '(' {
				depth++
			} else if C[i] == ')' {
				depth--
				if depth == 0 {
					// This closes the opening at ini
					// If this is the end, the whole thing is (expr)
					if i == fim {
						// Strip outer parens
						return ExpressaoParaArvore(C, ini+1, fim-1)
					}
					break
				}
			}
		}
		// Now find operator at depth 1 (relative to our starting paren)
		depth = 0
		for i := ini; i <= fim; i++ {
			if C[i] == '(' {
				depth++
			} else if C[i] == ')' {
				depth--
			} else if depth == 1 && (C[i] == '+' || C[i] == '-' || C[i] == '*' || C[i] == '/') {
				splitPos = i
				break
			}
		}
		if splitPos != -1 {
			return &No{
				Op:  C[splitPos],
				Esq: ExpressaoParaArvore(C, ini+1, splitPos-1),
				Dir: ExpressaoParaArvore(C, splitPos+1, fim-1),
			}
		}
		// No operator found at depth 1, strip outer parens
		return ExpressaoParaArvore(C, ini+1, fim-1)
	}
	// It's a digit
	return &No{Op: '@', E: int(C[ini] - '0')}
}

func escrever(T *No) {
	if T != nil {
		escrever(T.Esq)
		escrever(T.Dir)
		if T.Op != '@' {
			fmt.Printf("%c ", T.Op)
		} else {
			fmt.Printf("%d ", T.E)
		}
	}
}

func destroi(T **No) {
	if *T != nil {
		destroi(&(*T).Esq)
		destroi(&(*T).Dir)
		*T = nil
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
		T := ExpressaoParaArvore(line, 0, len(line)-1)
		escrever(T)
		fmt.Println()
		destroi(&T)
	}
}
