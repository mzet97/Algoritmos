package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
)

type E1 struct {
	Nome   []byte
	Idade  int
}

type E2 struct {
	Turma int
	Aluno *E1
	Prox *E2
}

var V []*E1
var L *E2

func TestarMemoria(t int) {
	if t == 1 {
		fmt.Printf("%d %d %d\n", V[0].Idade, V[1].Idade, V[2].Idade)
	}
	if t == 2 {
		fmt.Printf("%c %c %c %c\n", V[0].Nome[0], V[0].Nome[1], V[0].Nome[2], V[0].Nome[3])
	}
	if t == 3 {
		fmt.Printf("%c %c %c\n", V[1].Nome[0], V[1].Nome[1], V[1].Nome[2])
	}
	if t == 4 {
		fmt.Printf("%c %c %c\n", V[2].Nome[0], V[2].Nome[1], V[2].Nome[2])
	}
	if t == 5 {
		fmt.Printf("%d %d %d %d\n", L.Turma, L.Prox.Turma, L.Prox.Prox.Turma, L.Prox.Prox.Prox.Turma)
	}
	if t == 6 {
		if L.Prox.Prox.Prox.Prox == nil {
			fmt.Printf("1\n")
		} else {
			fmt.Printf("0\n")
		}
	}
	if t == 7 {
		fmt.Printf("%d %d %d %d\n", L.Aluno.Idade, L.Prox.Aluno.Idade, L.Prox.Prox.Aluno.Idade, L.Prox.Prox.Prox.Aluno.Idade)
	}
}

func main() {
	V = make([]*E1, 3)
	V[0] = &E1{Nome: []byte("JOAO"), Idade: 5}
	V[1] = &E1{Nome: []byte("LEO"), Idade: 8}
	V[2] = &E1{Nome: []byte("LIA"), Idade: 9}

	n4 := &E2{Turma: 2, Aluno: V[2], Prox: nil}
	n3 := &E2{Turma: 2, Aluno: V[1], Prox: n4}
	n2 := &E2{Turma: 1, Aluno: V[2], Prox: n3}
	n1 := &E2{Turma: 1, Aluno: V[0], Prox: n2}
	L = n1

	scanner := bufio.NewScanner(os.Stdin)
	for scanner.Scan() {
		line := scanner.Text()
		if line == "" {
			continue
		}
		t, err := strconv.Atoi(line)
		if err != nil {
			continue
		}
		TestarMemoria(t)
	}
}
