package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
)

func acimaMedia(l []int) int {
	if len(l) == 0 { return 0 }
	soma := 0
	for _, v := range l { soma += v }
	media := float64(soma) / float64(len(l))
	count := 0
	for _, v := range l {
		if float64(v) > media { count++ }
	}
	return count
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 2*1024*1024), 2*1024*1024)
	for {
		sn, err := bib1.Entrada(scanner)
		if err != nil { break }
		if sn == "@entradagrande" {
			l := make([]int, 1000000)
			for i := range l { l[i] = i }
			fmt.Println(acimaMedia(l))
		} else {
			n, _ := strconv.Atoi(sn)
			l := make([]int, n)
			for i := 0; i < n; i++ {
				s, err := bib1.Entrada(scanner)
				if err != nil { return }
				l[i], _ = strconv.Atoi(s)
			}
			fmt.Println(acimaMedia(l))
		}
	}
}
