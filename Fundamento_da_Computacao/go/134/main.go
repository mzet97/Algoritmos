package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"math"
	"os"
	"strconv"
)

func menorAngulo(H, M int) float64 {
	minuteAngle := float64(M) * 6.0
	hourAngle := float64(H%12)*30.0 + float64(M)*0.5
	diff := math.Abs(hourAngle - minuteAngle)
	if diff > 180.0 {
		diff = 360.0 - diff
	}
	return diff
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sh, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		sm, err := bib1.Entrada(scanner)
		if err != nil {
			break
		}
		h, _ := strconv.Atoi(sh)
		m, _ := strconv.Atoi(sm)
		fmt.Println(bib1.PythonFloat(menorAngulo(h, m)))
	}
}
