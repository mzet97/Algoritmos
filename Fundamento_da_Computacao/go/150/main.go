package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"os"
	"strconv"
	"strings"
)

func contaUnicos(P string) int {
	count := make(map[byte]int)
	for i := 0; i < len(P); i++ {
		count[P[i]]++
	}
	result := 0
	for _, v := range count {
		if v == 1 {
			result++
		}
	}
	return result
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 2*1024*1024), 2*1024*1024)
	for {
		nStr, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		N, _ := strconv.Atoi(nStr)
		var P string
		if N >= 0 {
			P, err = bib1.Entrada(scanner)
			if err != nil {
				break
			}
		} else {
			P = strings.Repeat("abcde", 10000)
		}
		fmt.Println(contaUnicos(P))
	}
}
