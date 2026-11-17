package bib1

import (
	"bufio"
	"errors"
	"fmt"
	"math"
	"strconv"
	"strings"
)

var ErrEOF = errors.New("EOF")

// Entrada lê uma linha de stdin ignorando linhas vazias.
func Entrada(scanner *bufio.Scanner) (string, error) {
	for {
		if !scanner.Scan() {
			return "", ErrEOF
		}
		line := strings.TrimSpace(scanner.Text())
		if line != "" {
			return line, nil
		}
	}
}

// PythonFloat formata float64 no estilo do Python (shortest round-trip).
// Python uses fixed notation unless exponent >= 16 or < -4.
func PythonFloat(x float64) string {
	if math.IsInf(x, 1) {
		return "inf"
	}
	if math.IsInf(x, -1) {
		return "-inf"
	}
	if math.IsNaN(x) {
		return "nan"
	}
	// Find the shortest representation that round-trips
	for prec := 1; prec <= 17; prec++ {
		s := fmt.Sprintf("%.*g", prec, x)
		back, err := strconv.ParseFloat(s, 64)
		if err == nil && back == x {
			// If it contains e/E notation, convert to fixed if Python wouldn't use sci notation
			if strings.ContainsAny(s, "eE") {
				// Reformat as fixed-point with enough precision
				fixed := strconv.FormatFloat(x, 'f', -1, 64)
				if !strings.Contains(fixed, ".") {
					fixed += ".0"
				}
				return fixed
			}
			if !strings.Contains(s, ".") {
				s += ".0"
			}
			return s
		}
	}
	s := strconv.FormatFloat(x, 'f', -1, 64)
	if !strings.Contains(s, ".") {
		s += ".0"
	}
	return s
}
