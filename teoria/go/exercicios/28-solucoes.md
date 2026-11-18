---
layout: default
---

# Aula 28 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../28.md#25-exercicios-obrigatorios).

```go
package main

import (
	"bufio"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"time"
)

func contarPalavrasReadFile(caminho string) (int, error) {
	dados, err := os.ReadFile(caminho)
	if err != nil {
		return 0, err
	}
	return len(strings.Fields(string(dados))), nil
}

func contarPalavrasBufio(caminho string, tamanhoBuffer int) (int, error) {
	f, err := os.Open(caminho)
	if err != nil {
		return 0, err
	}
	defer f.Close()
	scanner := bufio.NewScanner(bufio.NewReaderSize(f, tamanhoBuffer))
	scanner.Buffer(make([]byte, tamanhoBuffer), 1<<20)
	total := 0
	for scanner.Scan() {
		total += len(strings.Fields(scanner.Text()))
	}
	return total, scanner.Err()
}

func medir(nome string, fn func() (int, error)) int {
	inicio := time.Now()
	n, err := fn()
	if err != nil {
		panic(err)
	}
	fmt.Printf("%s: %d palavras em %d ms\n", nome, n, time.Since(inicio).Milliseconds())
	return n
}

func main() {
	caminho := filepath.Join(os.TempDir(), "aula28_grande.txt")
	var escrita strings.Builder
	for i := 0; i < 60000; i++ {
		fmt.Fprintf(&escrita, "linha %d com algumas palavras para contar\n", i)
	}
	if err := os.WriteFile(caminho, []byte(escrita.String()), 0o644); err != nil {
		panic(err)
	}
	defer os.Remove(caminho)
	info, _ := os.Stat(caminho)
	fmt.Println("arquivo MiB:", info.Size()/(1<<20))

	// Aquecimento: a primeira leitura paga o page cache pelas outras (exercicio 6).
	_, _ = contarPalavrasReadFile(caminho)
	a := medir("ReadFile", func() (int, error) { return contarPalavrasReadFile(caminho) })
	b := medir("bufio 4 KiB", func() (int, error) { return contarPalavrasBufio(caminho, 4096) })
	c := medir("bufio 64 KiB", func() (int, error) { return contarPalavrasBufio(caminho, 65536) })
	fmt.Println("contagens iguais:", a == b && b == c)
	fmt.Println("testes ok")
}
```

2. Entre 4 KiB e 64 KiB, neste arquivo, a diferenca cabe no ruido: o trabalho por linha (`Fields`) domina o custo da syscall amortizada. Buffer muda o jogo quando o processamento por pedaco e quase zero (copia pura); quando ha trabalho real por linha, ele some na medicao — medir antes de tunar, como sempre.

3. `mmap` ajuda mais em: leitura aleatoria repetida de um arquivo grande demais para carregar (indices, bancos embarcados), onde o sistema operacional pagina sob demanda e o cache e compartilhado entre processos. Para varredura sequencial unica, `bufio` ja entrega o mesmo com erro tratavel.

4. Riscos operacionais de `mmap`: arquivo truncado por outro processo vira `SIGBUS` no acesso (sinal, nao `error` Go); portabilidade de flags entre Linux/Windows/macOS; e vida util do mapeamento amarrada ao descritor. Em servico multi-plataforma, isso costuma custar mais que o ganho.

6. Page cache distorce assim: a primeira leitura mede o disco; todas as seguintes medem a RAM do kernel. Benchmark de leitura que nao declara aquecimento (nem `drop_caches`, onde permitido) esta comparando temperaturas diferentes.

**Autoverificacao:** as tres contagens iguais sobre o mesmo arquivo, com aquecimento feito antes de qualquer relogio.
