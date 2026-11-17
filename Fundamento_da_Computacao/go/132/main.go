package main

import (
	"bufio"
	"fmt"
	"fundamento_go/bib1"
	"math"
	"os"
	"strconv"
)

func tv(d, pl, ph int) (float64, float64) {
	diagCm := float64(d) * 2.54
	ratio := float64(pl) / float64(ph)
	h := diagCm / math.Sqrt(1+ratio*ratio)
	w := h * ratio
	w = math.Round(w*10) / 10
	h = math.Round(h*10) / 10
	return w, h
}

func fmtFloat(f float64) string {
	if f == float64(int(f)) && math.Abs(f) < 1e15 { return strconv.Itoa(int(f)) }
	return bib1.PythonFloat(f)
}

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	for {
		sd, err := bib1.Entrada(scanner)
		if err != nil { break }
		spl, err := bib1.Entrada(scanner)
		if err != nil { break }
		sph, err := bib1.Entrada(scanner)
		if err != nil { break }
		d, _ := strconv.Atoi(sd)
		pl, _ := strconv.Atoi(spl)
		ph, _ := strconv.Atoi(sph)
		w, h := tv(d, pl, ph)
		fmt.Printf("(%s, %s)\n", fmtFloat(w), fmtFloat(h))
	}
}
