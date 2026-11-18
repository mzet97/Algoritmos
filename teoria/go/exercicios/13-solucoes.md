---
layout: default
---

# Aula 13 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../13.md#12-exercicios-obrigatorios).

```go
package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"strings"
)

type Config struct {
	Host  string `json:"host"`
	Porta int    `json:"porta"`
	Debug bool   `json:"debug"`
}

func main() {
	caminho := filepath.Join(os.TempDir(), "aula13_go.txt")
	conteudo := "linha um\nlinha dois\nlinha tres\n"
	if err := os.WriteFile(caminho, []byte(conteudo), 0o644); err != nil {
		panic(err)
	}
	defer os.Remove(caminho)

	inteiro, err := os.ReadFile(caminho)
	if err != nil {
		panic(err)
	}
	fmt.Printf("ReadFile: %d bytes\n", len(inteiro))

	arquivo, err := os.Open(caminho)
	if err != nil {
		panic(err)
	}
	scanner := bufio.NewScanner(arquivo)
	linhas := 0
	for scanner.Scan() {
		linhas++
	}
	if err := scanner.Err(); err != nil {
		panic(err)
	}
	arquivo.Close()
	fmt.Println("Scanner: linhas =", linhas)

	original := Config{Host: "localhost", Porta: 8080, Debug: true}
	dados, err := json.Marshal(original)
	if err != nil {
		panic(err)
	}
	fmt.Println("JSON:", string(dados))

	var volta Config
	if err := json.Unmarshal(dados, &volta); err != nil {
		panic(err)
	}
	fmt.Printf("volta: %+v\n", volta)

	juntado := filepath.Join("etc", "app", "config.json")
	fmt.Println("Join:", juntado, "| separador da plataforma:", string(filepath.Separator))
	if linhas != 3 || volta != original || !strings.HasSuffix(juntado, "config.json") {
		panic("resultados inesperados")
	}
	fmt.Println("testes ok")
}
```

5. Concatenar `"etc/" + "app/" + nome` quebra com separador errado noutra plataforma, barras duplicadas e `..` nao resolvidos. `filepath.Join` monta com o separador local e aplica `Clean`: o caminho que sai e o caminho canonico que o sistema vai usar, e nao uma esperanca tipografica.

**Autoverificacao:** as duas leituras concordando (bytes e 3 linhas), o JSON de ida e volta igual (`volta == original`) e nenhum caminho montado com `+` de barras no seu codigo.
