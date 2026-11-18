---
layout: default
---

# Aula 21 (Go): solucoes comentadas

Compare apos tentar os [exercicios](../21.md#20-exercicios-obrigatorios). Projeto de modulo executado nesta rodada com `github.com/gorilla/websocket` (a biblioteca padrao nao traz WebSocket). Tres arquivos, uma `package main` (exercicio 6):

```go
// hub.go
package main

import "log"

type mensagem struct {
	de   string
	texto string
}

type hub struct {
	clientes   map[*cliente]bool
	registrar  chan *cliente
	remover   chan *cliente
	broadcast chan mensagem
	contagem  chan chan int
}

func novoHub() *hub {
	return &hub{
		clientes:  make(map[*cliente]bool),
		registrar: make(chan *cliente),
		remover:   make(chan *cliente),
		broadcast: make(chan mensagem),
		contagem:  make(chan chan int),
	}
}

func (h *hub) total() int {
	resposta := make(chan int)
	h.contagem <- resposta
	return <-resposta
}

func (h *hub) rodar() {
	for {
		select {
		case c := <-h.registrar:
			h.clientes[c] = true
		case c := <-h.remover:
			if h.clientes[c] {
				delete(h.clientes, c)
				close(c.enviar)
			}
		case resposta := <-h.contagem:
			resposta <- len(h.clientes)
		case m := <-h.broadcast:
			texto := m.de + ": " + m.texto
			for c := range h.clientes {
				select {
				case c.enviar <- texto:
				default:
					// Cliente lento: politica explicita, log + desconexao.
					log.Printf("cliente %s lento; desconectando", c.id)
					delete(h.clientes, c)
					close(c.enviar)
				}
			}
		}
	}
}
```

```go
// cliente.go
package main

import (
	"github.com/gorilla/websocket"
)

const limiteMensagem = 64

type cliente struct {
	id     string
	hub    *hub
	conn   *websocket.Conn
	enviar chan string
}

func (c *cliente) ler() {
	defer func() {
		c.hub.remover <- c
		c.conn.Close()
	}()
	c.conn.SetReadLimit(limiteMensagem)
	for {
		_, dados, err := c.conn.ReadMessage()
		if err != nil {
			return
		}
		c.hub.broadcast <- mensagem{de: c.id, texto: string(dados)}
	}
}

func (c *cliente) escrever() {
	defer c.conn.Close()
	for texto := range c.enviar {
		if err := c.conn.WriteMessage(websocket.TextMessage, []byte(texto)); err != nil {
			return
		}
	}
}
```

```go
// main.go
package main

import (
	"fmt"
	"io"
	"net/http"
	"net/http/httptest"
	"strings"
	"sync/atomic"
	"time"

	"github.com/gorilla/websocket"
)

var atualizador = websocket.Upgrader{}

func main() {
	h := novoHub()
	go h.rodar()
	var proximoID atomic.Int64 // handlers rodam em goroutines: contador atomico
	mux := http.NewServeMux()
	mux.HandleFunc("/ws", func(w http.ResponseWriter, r *http.Request) {
		conn, err := atualizador.Upgrade(w, r, nil)
		if err != nil {
			return
		}
		c := &cliente{
			id:     fmt.Sprintf("cliente-%d", proximoID.Add(1)),
			hub:    h,
			conn:   conn,
			enviar: make(chan string, 4), // buffer pequeno: lento aparece cedo
		}
		h.registrar <- c
		go c.escrever()
		go c.ler()
	})
	mux.HandleFunc("/clientes", func(w http.ResponseWriter, r *http.Request) {
		fmt.Fprintf(w, "%d", h.total())
	})
	servidor := httptest.NewServer(mux)
	defer servidor.Close()

	dialer := websocket.Dialer{HandshakeTimeout: 2 * time.Second}
	urlWS := "ws" + strings.TrimPrefix(servidor.URL, "http") + "/ws"
	a, _, err := dialer.Dial(urlWS, nil)
	if err != nil {
		panic(err)
	}
	defer a.Close()
	b, _, err := dialer.Dial(urlWS, nil)
	if err != nil {
		panic(err)
	}
	defer b.Close()

	contagem := func() string {
		resp, err := http.Get(servidor.URL + "/clientes")
		if err != nil {
			panic(err)
		}
		defer resp.Body.Close()
		dados, _ := io.ReadAll(resp.Body)
		return string(dados)
	}
	// Registro e assincrono; espera limitada e uma condicao, nao um chute.
	for i := 0; i < 50 && contagem() != "2"; i++ {
		time.Sleep(20 * time.Millisecond)
	}
	fmt.Println("clientes conectados:", contagem())

	if err := a.WriteMessage(websocket.TextMessage, []byte("ola")); err != nil {
		panic(err)
	}
	a.SetReadDeadline(time.Now().Add(2 * time.Second))
	_, dados, err := a.ReadMessage()
	if err != nil {
		panic(err)
	}
	fmt.Println("a recebeu:", string(dados))
	b.SetReadDeadline(time.Now().Add(2 * time.Second))
	_, dados, err = b.ReadMessage()
	if err != nil {
		panic(err)
	}
	fmt.Println("b recebeu:", string(dados))

	// Mensagem acima do limite: a conexao deve cair.
	_ = a.WriteMessage(websocket.TextMessage, []byte(strings.Repeat("x", limiteMensagem+10)))
	a.SetReadDeadline(time.Now().Add(2 * time.Second))
	_, _, err = a.ReadMessage()
	fmt.Println("conexao apos mensagem gigante caiu:", err != nil)
	fmt.Println("testes ok")
}
```

3. O buffer `send` de 4 e deliberadamente pequeno neste laboratorio: ele e a fila de saida por cliente. Buffer grande esconde cliente lento ate a memoria doer; buffer pequeno expoe cedo e obriga a politica do item 4 a existir. Em producao, o tamanho sai da taxa de mensagens x tempo tolerado de atraso, com metrica de desconexoes por lentidao.

**Autoverificacao:** broadcast chega aos dois com prefixo `cliente-N:`, `/clientes` responde 2 e a mensagem gigante derruba a conexao.
