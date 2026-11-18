package main

import "fmt"

const nos = 3

type entrada struct {
	valor   string
	relogio [nos]int
}

type replica struct {
	id    int
	viva  bool
	dados map[string]entrada
}

// domina: a >= b em tudo e > em algo.
func domina(a, b [nos]int) bool {
	maior := false
	for i := 0; i < nos; i++ {
		if a[i] < b[i] {
			return false
		}
		maior = maior || a[i] > b[i]
	}
	return maior
}

func escrever(replicas *[nos]replica, coord int, chave, valor string) bool {
	acks := 0
	for i := range replicas {
		if !replicas[i].viva {
			continue
		}
		e := replicas[i].dados[chave]
		if i == coord {
			e.relogio[i]++
		}
		e.valor = valor
		replicas[i].dados[chave] = e
		acks++
	}
	return acks >= 2
}

func ler(replicas *[nos]replica, chave string) (entrada, bool, bool) {
	var respostas []entrada
	for i := range replicas {
		if !replicas[i].viva {
			continue
		}
		e, ok := replicas[i].dados[chave]
		if ok {
			respostas = append(respostas, e)
		}
	}
	if len(respostas) < 2 {
		return entrada{}, false, false
	}
	melhor := respostas[0]
	conflito := false
	for _, e := range respostas[1:] {
		switch {
		case e.relogio == melhor.relogio:
		case domina(e.relogio, melhor.relogio):
			melhor = e
		case !domina(melhor.relogio, e.relogio):
			conflito = true
		}
	}
	// Reparo de leitura: empurra a vencedora para as atrasadas.
	for i := range replicas {
		if !replicas[i].viva {
			continue
		}
		e := replicas[i].dados[chave]
		if e.relogio != melhor.relogio {
			replicas[i].dados[chave] = melhor
		}
	}
	return melhor, true, conflito
}

func main() {
	var replicas [nos]replica
	for i := range replicas {
		replicas[i] = replica{id: i, viva: true, dados: map[string]entrada{}}
	}

	if !escrever(&replicas, 0, "k", "111") {
		panic("fase1: escrita deveria confirmar")
	}
	m, ok, conflito := ler(&replicas, "k")
	if !ok || conflito || m.valor != "111" {
		panic("fase1: leitura deveria ser limpa")
	}
	fmt.Println("fase1 quorum limpo: k=111")

	// Particao: lado {2} sozinho nao escreve nem le.
	replicas[2].viva = false
	if !escrever(&replicas, 0, "k", "222") {
		panic("fase2: maioria {0,1} deveria confirmar")
	}
	if _, ok, _ := ler(&replicas, "k"); !ok {
		panic("fase2: maioria deveria ler")
	}
	replicas[0].viva = false
	if _, ok, _ := ler(&replicas, "k"); ok {
		panic("fase2: minoria de 1 nao pode ler por quorum")
	}
	fmt.Println("fase2 minoria sem quorum: escrita e leitura recusadas")
	replicas[0].viva = true

	// Cura + reparo: a atrasada recebe a vencedora na leitura.
	replicas[2].viva = true
	m, ok, conflito = ler(&replicas, "k")
	if !ok || conflito || m.valor != "222" {
		panic("fase3: cura deveria convergir em 222")
	}
	if replicas[2].dados["k"].valor != "222" {
		panic("fase3: reparo deveria atualizar a atrasada")
	}
	fmt.Println("fase3 pos-cura: k=222, atrasada reparada na leitura")
	fmt.Println("testes ok")
}
