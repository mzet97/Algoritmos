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

func ConstroiFila(F *Fila) {
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

func TamanhoFila(F *Fila) int {
	return len(F.dados) - F.front
}

func AlternarPosNeg(num []int, n int) {
	pos := Fila{}
	neg := Fila{}
	ConstroiFila(&pos)
	ConstroiFila(&neg)
	for i := 0; i < n; i++ {
		if num[i] > 0 {
			Enfileira(&pos, num[i])
		} else {
			Enfileira(&neg, num[i])
		}
	}
	for TamanhoFila(&pos) > 0 && TamanhoFila(&neg) > 0 {
		if TamanhoFila(&pos) > 0 {
			fmt.Printf("%d ", Desenfileira(&pos))
		}
		if TamanhoFila(&neg) > 0 {
			fmt.Printf("%d ", Desenfileira(&neg))
		}
	}
	for TamanhoFila(&pos) > 0 {
		fmt.Printf("%d ", Desenfileira(&pos))
	}
	for TamanhoFila(&neg) > 0 {
		fmt.Printf("%d ", Desenfileira(&neg))
	}
	fmt.Println()
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
		if n > -1 {
			scanner.Scan()
			dataLine := strings.TrimSpace(scanner.Text())
			parts := strings.Fields(dataLine)
			num := make([]int, n)
			for i := 0; i < n; i++ {
				num[i], _ = strconv.Atoi(parts[i])
			}
			AlternarPosNeg(num, n)
		} else {
			if n == -1 {
				n = 100000
				num := make([]int, n)
				s := 1
				for i := 0; i < n; i++ {
					num[i] = s * (i + 1)
					s = -s
				}
				AlternarPosNeg(num, n)
			} else if n == -2 {
				n = 100000
				num := make([]int, n)
				for i := 0; i < n/2; i++ {
					num[i] = (i + 1)
				}
				for i := n / 2; i < n; i++ {
					num[i] = -(i + 1)
				}
				AlternarPosNeg(num, n)
			} else {
				n = 100000
				num := make([]int, n)
				for i := 0; i < n/2; i++ {
					num[i] = -(i + 1)
				}
				for i := n / 2; i < n; i++ {
					num[i] = (i + 1)
				}
				AlternarPosNeg(num, n)
			}
		}
	}
}
