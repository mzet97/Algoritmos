---
layout: default
---

# Aula 23a - Framing, replicação e falhas sob controle

---

## Objetivo da Aula

Construir um serviço replicado de pares chave-valor com enquadramento binário, idempotência por chave de operação e log durável — e quebrá-lo de propósito com atraso, duplicação, truncamento, queda e reinício. Pré-requisitos: aulas 08b a 22. Não precisa de consenso, quorum, TLS nem orquestrador.

Ao final, você deverá: implementar framing com prefixo de tamanho, replicar com confirmação, tratar falhas sob controle, e estimar limites.
Mediremos com níveis de proficiência:
- **Nível 1 — Recordar:** `definir framing, réplica, confirmação e falha controlada`;
- **Nível 2 — Aplicar:** `diagnosticar leitura parcial e corrigir com framing`;
- **Nível 3 — Projetar:** `projetar o protocolo justificando framing, réplicas e limites`.

## 1. TCP é fluxo de bytes: enquadre com tamanho, não com esperança

Linha terminada em `\n` funciona até o primeiro `\n` dentro do valor. Enquadramento binário honesto: 4 bytes big-endian de comprimento + payload JSON, com teto (64 KiB aqui). Leitura parcial é o caso comum: `io.ReadFull` repete até completar ou falhar — e `io.ErrUnexpectedEOF` no meio significa truncamento, que se descarta sem contaminar a próxima mensagem (cada conexão trata um fluxo; erro fecha a conexão, não o servidor).

## 2. Idempotência antes de retry

Regra de ouro: nunca repita sem chave. Cada operação carrega `id` único; o servidor guarda `vistos[id] = resultado` e reenvios devolvem o guardado sem reaplicar. Retry sem idempotência transforma queda em duplicação — e duplicação de `put` com valor diferente é corrupção silenciosa.

## 3. Log antes de responder, replay ao nascer

Toda mutação: valida → anexa ao log com `Sync` → aplica em memória → responde. Na inicialização, o servidor relê o log inteiro: o que foi respondido como `ok` está no disco, logo está de volta na memória. `fsync` por registro é lento e honesto; agrupar é otimização futura, documentada como tal.

## 4. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/23a_distribuido.go) *é* o programa completo (longo demais para transcrever sem divergir — o arquivo é a única fonte). Só biblioteca padrão; o cenário de falhas roda sozinho:

```bash
go vet teoria/go/exemplos/23a_distribuido.go && go run teoria/go/exemplos/23a_distribuido.go
```

```go
package main

import (
	"bufio"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"io"
	"net"
	"os"
	"sync"
	"time"
)

const maxQuadro = 64 * 1024 // teto do enquadramento

type Pedido struct {
	ID    string `json:"id"`
	Op    string `json:"op"`
	Chave string `json:"chave,omitempty"`
	Valor string `json:"valor,omitempty"`
}

type Resposta struct {
	OK       bool   `json:"ok"`
	Result   string `json:"resultado,omitempty"`
	Repetido bool   `json:"repetido,omitempty"`
	Erro     string `json:"erro,omitempty"`
}

// escreveQuadro enquadra: 4 bytes big-endian + payload, com teto.
func escreveQuadro(w io.Writer, payload []byte) error {
	if len(payload) > maxQuadro {
		return fmt.Errorf("quadro acima do teto: %d", len(payload))
	}
	var cab [4]byte
	binary.BigEndian.PutUint32(cab[:], uint32(len(payload)))
	if _, err := w.Write(cab[:]); err != nil {
		return err
	}
	_, err := w.Write(payload)
	return err
}

// leQuadro le com framing: parcial e normal, truncado e erro.
func leQuadro(r io.Reader) ([]byte, error) {
	var cab [4]byte
	if _, err := io.ReadFull(r, cab[:]); err != nil {
		return nil, err
	}
	n := binary.BigEndian.Uint32(cab[:])
	if n > maxQuadro {
		return nil, fmt.Errorf("quadro declara %d bytes, teto %d", n, maxQuadro)
	}
	payload := make([]byte, n)
	if _, err := io.ReadFull(r, payload); err != nil {
		return nil, fmt.Errorf("truncado: %w", err)
	}
	return payload, nil
}

// No e um par chave-valor com log duravel e dedup por ID.
type No struct {
	mu       sync.Mutex
	dados    map[string]string
	vistos   map[string]string // id -> resultado (idempotencia)
	log      *os.File
	buf      *bufio.Writer
	ouvindo  net.Listener
	par      string // "host:porta" da replica, ou ""
	atraso   time.Duration
	fechar   chan struct{}
	espera   sync.WaitGroup
	recusado int64
}

func novoNo(dir, nome, par string, atraso time.Duration) (*No, error) {
	arq, err := os.OpenFile(dir+"/"+nome+".log", os.O_CREATE|os.O_RDWR|os.O_APPEND, 0600)
	if err != nil {
		return nil, err
	}
	n := &No{dados: map[string]string{}, vistos: map[string]string{}, par: par, atraso: atraso, fechar: make(chan struct{})}
	// Replay: o que foi respondido como ok esta no disco, volta a memoria.
	if _, err := arq.Seek(0, io.SeekStart); err != nil {
		return nil, err
	}
	dec := json.NewDecoder(bufio.NewReader(arq))
	for {
		var reg Pedido
		if err := dec.Decode(&reg); err != nil {
			break
		}
		if reg.Op == "put" {
			n.dados[reg.Chave] = reg.Valor
			n.vistos[reg.ID] = reg.Valor
		}
	}
	n.log = arq
	n.buf = bufio.NewWriter(arq)
	return n, nil
}

func (n *No) persiste(p Pedido) error {
	l, err := json.Marshal(p)
	if err != nil {
		return err
	}
	l = append(l, '\n')
	if _, err := n.buf.Write(l); err != nil {
		return err
	}
	if err := n.buf.Flush(); err != nil {
		return err
	}
	return n.log.Sync() // duravel antes de responder
}

func (n *No) replica(p Pedido) {
	if n.par == "" {
		return
	}
	if n.atraso > 0 {
		time.Sleep(n.atraso)
	}
	conn, err := net.DialTimeout("tcp", n.par, 2*time.Second)
	if err != nil {
		return // par fora do ar: degrada, nao trava
	}
	defer conn.Close()
	_ = conn.SetDeadline(time.Now().Add(2 * time.Second))
	payload, _ := json.Marshal(p)
	if err := escreveQuadro(conn, payload); err != nil {
		return
	}
	_, _ = leQuadro(conn) // melhor esforco: resposta ignorada
}

func (n *No) trata(p Pedido) Resposta {
	if p.ID == "" || (p.Op != "put" && p.Op != "get") {
		return Resposta{Erro: "pedido invalido"}
	}
	n.mu.Lock()
	if v, ok := n.vistos[p.ID]; ok {
		n.mu.Unlock()
		return Resposta{OK: true, Result: v, Repetido: true}
	}
	if p.Op == "get" {
		v := n.dados[p.Chave]
		n.mu.Unlock()
		return Resposta{OK: true, Result: v}
	}
	if err := n.persiste(p); err != nil {
		n.mu.Unlock()
		return Resposta{Erro: "log: " + err.Error()}
	}
	n.dados[p.Chave] = p.Valor
	n.vistos[p.ID] = p.Valor
	n.mu.Unlock()
	n.replica(p) // fora do lock: I/O nao serializa o estado
	return Resposta{OK: true, Result: p.Valor}
}

func (n *No) atende(conn net.Conn) {
	defer conn.Close()
	_ = conn.SetDeadline(time.Now().Add(10 * time.Second))
	for {
		payload, err := leQuadro(conn)
		if err != nil {
			return // truncado ou EOF: fecha a conexao, nao o servidor
		}
		var p Pedido
		if err := json.Unmarshal(payload, &p); err != nil {
			resp, _ := json.Marshal(Resposta{Erro: "json invalido"})
			_ = escreveQuadro(conn, resp)
			continue
		}
		resp, _ := json.Marshal(n.trata(p))
		if err := escreveQuadro(conn, resp); err != nil {
			return
		}
	}
}

func (n *No) serve(l net.Listener) {
	n.ouvindo = l
	for {
		conn, err := l.Accept()
		if err != nil {
			select {
			case <-n.fechar:
				return
			default:
				continue
			}
		}
		n.espera.Add(1)
		go func() {
			defer n.espera.Done()
			n.atende(conn)
		}()
	}
}

func (n *No) para() {
	close(n.fechar)
	if n.ouvindo != nil {
		n.ouvindo.Close()
	}
	n.espera.Wait()
	_ = n.buf.Flush()
	_ = n.log.Close()
}
```

O programa principal (`main`) sobe dois nós em portas efêmeras no mesmo processo — TCP real via loopback, com as mesmas syscalls do deploy — e executa o roteiro: escrita, leitura nos dois, duplicata com valor diferente (vale a primeira), truncamento (bytes parciais + fechamento: servidor descarta, conexão seguinte funciona), queda da réplica, escrita degradada, reinício com o mesmo log (replay que ignora cauda truncada), reenvio com o mesmo ID (converge sem duplicar), atraso de 300 ms no encaminhamento (resposta lenta, sem travar) e igualdade final dos estados. Como a réplica renasce em outra porta, o teste atualiza o par do primário explicitamente — descoberta automática (gossip, failure detector) é trabalho futuro declarado, não comportamento fingido. Cada passo tem `assert` implícito: qualquer desvio aborta com mensagem. A saída termina com `DISTRIBUIDO_OK` e os tempos de cada fase.

### O que a memória e as goroutines fazem aqui

Cada conexão aceita vira uma goroutine que morre ao fechar a conexão; o estado compartilhado vive atrás de um `sync.Mutex` com seção crítica mínima (checar+decidir+registrar), e o encaminhamento de rede acontece *fora* do lock para não serializar I/O. O log é o único estado que sobrevive ao processo — todo o resto é reconstruído dele no boot.

## 5. Erros comuns e implicações defensivas

1. **Responder antes de persistir:** queda entre resposta e disco é mentira contada ao cliente. Log com `fsync` primeiro, sempre.
2. **Retry sem ID:** transforma queda em duplicação. O ID nasce no cliente e atravessa todos os reenvios.
3. **Dedup só em RAM:** reinício esquece. O conjunto de IDs vistos mora no log, reconstruído no replay.
4. **Timeout ausente no encaminhamento:** par lento vira cliente travado. 2 s aqui; declare e meça o seu.
5. **Aceitar-e-responder na mesma thread do `accept`:** deadlock mútuo mascarado por timeout. Concorrência por conexão é correção, não luxo.
6. **Achar que dois nós provam consenso:** provam replicação com retry dirigido. Quorum, eleição e ordem total são outro capítulo.

## 6. Ponte com Python, Rust e C++

Python faz o mesmo roteiro com dois processos e `fsync` (aula 23a da trilha Python); Rust enquadra com `read_until` e testa fragmentação byte a byte (aula 14a da trilha Rust); C++ isola cliente lento e mede syscall (aulas 17a e 31). Em todas: enquadrar, limitar, idempotenciar, persistir antes de responder.

## 7. Exercícios obrigatórios

1. Desenhe o caminho de um `put` do cliente até os dois logs, marcando onde cada falha possível deixa o sistema e quem repete o quê.
2. Mate um nó no meio de 100 escritas e conte quantas precisam de reenvio, quantas voltam `repetido` e quantas aplicam de verdade.
3. Suba o teto de 64 KiB para 1 byte e preveja o que quebra primeiro. Confirme executando.
4. Desligue o `fsync` (troque por escrita sem sincronizar) e meça quanto "ganha" — depois explique por que o número é mentira em queda de energia.

Use [pistas graduais](exercicios/23a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/23a_solucoes.md) depois de registrar sua própria explicação.

## 8. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- explicar por que TCP não tem mensagem e como o enquadramento com teto resolve;
- desenhar o caminho validar → persistir → aplicar → responder e dizer o que cada inversão quebra;
- provar idempotência por reenvio com valor diferente;
- demonstrar queda, reinício com replay e convergência sem atalho;
- apontar o que o par de nós garante e o que ele explicitamente não garante;
- executar o exemplo com `go vet` limpo e confrontar cada `ok:` com o contrato.

**Próxima aula:** observabilidade e operação. O raciocínio desta aula — log, retry com ID, degradação explícita — é o que há para observar quando o distribuído falha às 3h da manhã.

---

### Como esta aula será avaliada

| Critério | Peso |
| -------- | ---: |
| Conceitos e vocabulário (nível 1) | 20% |
| Previsão de comportamento e diagnóstico de erros (nível 2) | 40% |
| Projeto e justificativa de decisões (nível 3) | 30% |
| Registro de comandos, saídas e interpretação | 10% |

**Aprovação:** nota mínima 7,0; pelo menos 60% dos pontos devem vir de previsão e diagnóstico (níveis 2 e 3), não apenas de código que executa.

**Para se aprofundar:**

1. *Designing Data-Intensive Applications*, Martin Kleppmann (O'Reilly, ISBN 978-1449373320); 2. Documentação oficial em [go.dev/doc](https://go.dev/doc/); 3. *Site Reliability Engineering* (Google, O'Reilly, ISBN 978-1491929124), caps. sobre monitoramento e SLOs.
