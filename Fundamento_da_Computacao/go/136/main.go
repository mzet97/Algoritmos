package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func imposto(l []int) float64 {
	total := 0.0
	for _, s := range l {
		if s <= 3000 {
			continue
		} else if s <= 8000 {
			total += float64(s-3000) * 0.15
		} else {
			total += 5000*0.15 + float64(s-8000)*0.27
		}
	}
	return total
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sn, err := bib1.Entrada(scanner)
		if err != nil { break }
		n, _ := strconv.Atoi(sn)
		l := make([]int, n)
		for i := 0; i < n; i++ {
			s, err := bib1.Entrada(scanner)
			if err != nil { return }
			l[i], _ = strconv.Atoi(s)
		}
		fmt.Println(imposto(l))
	}
}
