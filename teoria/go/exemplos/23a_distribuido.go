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
	"sync/atomic"
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
	mu      sync.Mutex
	dados   map[string]string
	vistos  map[string]string // id -> resultado (idempotencia)
	log     *os.File
	buf     *bufio.Writer
	ouvindo net.Listener
	par     string       // "host:porta" da replica, ou ""; escrita so via definePar
	atraso  atomic.Int64 // nanos de atraso injetado; atomico pois o teste muda em voo
	fechar  chan struct{}
	espera  sync.WaitGroup
	fechado bool
	fechaMu sync.Mutex
}

func novoNo(dir, nome, par string, atraso time.Duration) (*No, error) {
	arq, err := os.OpenFile(dir+"/"+nome+".log", os.O_CREATE|os.O_RDWR|os.O_APPEND, 0600)
	if err != nil {
		return nil, err
	}
	n := &No{dados: map[string]string{}, vistos: map[string]string{}, par: par, fechar: make(chan struct{})}
	n.atraso.Store(int64(atraso))
	// Replay: o que foi respondido como ok esta no disco, volta a memoria.
	// Cauda truncada (queda no meio do append) e ignorada: aquele pedido
	// nunca recebeu ack, logo o cliente vai repetir com o mesmo ID.
	if _, err := arq.Seek(0, io.SeekStart); err != nil {
		return nil, err
	}
	dec := json.NewDecoder(bufio.NewReader(arq))
	for {
		var reg Pedido
		if err := dec.Decode(&reg); err != nil {
			break
		}
		if reg.Op == "put" && reg.ID != "" {
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

// definePar troca o par sob lock: endereco de replica pode mudar no restart.
func (n *No) definePar(end string) {
	n.mu.Lock()
	n.par = end
	n.mu.Unlock()
}

func (n *No) replica(p Pedido) {
	n.mu.Lock()
	par := n.par
	n.mu.Unlock()
	if par == "" {
		return
	}
	if d := time.Duration(n.atraso.Load()); d > 0 {
		time.Sleep(d)
	}
	conn, err := net.DialTimeout("tcp", par, 2*time.Second)
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
			if err := escreveQuadro(conn, resp); err != nil {
				return
			}
			continue
		}
		resp, _ := json.Marshal(n.trata(p))
		if err := escreveQuadro(conn, resp); err != nil {
			return
		}
	}
}

func (n *No) serve(l net.Listener) {
	n.mu.Lock()
	n.ouvindo = l
	n.mu.Unlock()
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
	n.fechaMu.Lock()
	jaFechado := n.fechado
	n.fechado = true
	n.fechaMu.Unlock()
	if jaFechado {
		return
	}
	close(n.fechar)
	n.mu.Lock()
	l := n.ouvindo
	n.mu.Unlock()
	if l != nil {
		l.Close()
	}
	n.espera.Wait()
	_ = n.buf.Flush()
	_ = n.log.Close()
}

// ------------------------- roteiro -------------------------

func chama(endereco string, p Pedido) Resposta {
	conn, err := net.DialTimeout("tcp", endereco, 3*time.Second)
	if err != nil {
		panic(err)
	}
	defer conn.Close()
	_ = conn.SetDeadline(time.Now().Add(5 * time.Second))
	payload, _ := json.Marshal(p)
	if err := escreveQuadro(conn, payload); err != nil {
		panic(err)
	}
	resp, err := leQuadro(conn)
	if err != nil {
		panic(err)
	}
	var r Resposta
	if err := json.Unmarshal(resp, &r); err != nil {
		panic(err)
	}
	return r
}

func insiste(endereco string, tentativas int) {
	for i := 0; i < tentativas; i++ {
		conn, err := net.DialTimeout("tcp", endereco, 1*time.Second)
		if err == nil {
			conn.Close()
			return
		}
		time.Sleep(50 * time.Millisecond)
	}
	panic("servidor nao abriu: " + endereco)
}

func ouvePorta() (net.Listener, string) {
	l, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		panic(err)
	}
	return l, l.Addr().String()
}

func checa(cond bool, msg string) {
	if !cond {
		panic("FALHOU: " + msg)
	}
	fmt.Println("ok:", msg)
}

func main() {
	dir, err := os.MkdirTemp("", "aula23a")
	if err != nil {
		panic(err)
	}
	defer os.RemoveAll(dir)

	la, endA := ouvePorta()
	lb, endB := ouvePorta()
	a, err := novoNo(dir, "a", endB, 0)
	if err != nil {
		panic(err)
	}
	b, err := novoNo(dir, "b", endA, 0)
	if err != nil {
		panic(err)
	}
	go a.serve(la)
	go b.serve(lb)
	insiste(endA, 100)
	insiste(endB, 100)

	// 1-2. Escrita via A replica em B; leitura nos dois.
	r := chama(endA, Pedido{ID: "op1", Op: "put", Chave: "k1", Valor: "10"})
	checa(r.OK && !r.Repetido, "escrita responde ok")
	checa(chama(endA, Pedido{ID: "g1", Op: "get", Chave: "k1"}).Result == "10", "leitura no primario")
	checa(chama(endB, Pedido{ID: "g2", Op: "get", Chave: "k1"}).Result == "10", "replicacao na replica")

	// 3. Duplicata com valor diferente: vale a primeira.
	r = chama(endA, Pedido{ID: "op1", Op: "put", Chave: "k1", Valor: "999"})
	checa(r.OK && r.Repetido, "duplicata detectada")
	checa(chama(endA, Pedido{ID: "g3", Op: "get", Chave: "k1"}).Result == "10", "sem reaplicacao")

	// 4. Truncamento: cabecalho diz 100, corpo tem 5 e fecha.
	conn, err := net.DialTimeout("tcp", endA, 3*time.Second)
	if err != nil {
		panic(err)
	}
	_ = conn.SetDeadline(time.Now().Add(3 * time.Second))
	var cab [4]byte
	binary.BigEndian.PutUint32(cab[:], 100)
	_, _ = conn.Write(cab[:])
	_, _ = conn.Write([]byte("XXXXX"))
	_ = conn.Close() // fecha no meio: servidor descarta sem contaminar
	time.Sleep(200 * time.Millisecond)
	// Conexao seguinte funciona normalmente: sem contaminacao.
	r = chama(endA, Pedido{ID: "op2", Op: "put", Chave: "k2", Valor: "20"})
	checa(r.OK, "servidor saudavel apos truncamento")

	// 5-6. Queda da replica + escrita degradada.
	b.para()
	antes := time.Now()
	r = chama(endA, Pedido{ID: "op3", Op: "put", Chave: "k3", Valor: "30"})
	dt := time.Since(antes)
	checa(r.OK, "escrita com replica morta responde ok")
	fmt.Printf("degradado respondeu em %s (sem travar na replica morta)\n", dt.Round(time.Millisecond))
	checa(chama(endA, Pedido{ID: "g4", Op: "get", Chave: "k3"}).Result == "30", "dado presente no primario")

	// 7. Cauda truncada no log da replica (simula queda no meio do append).
	f, err := os.OpenFile(dir+"/b.log", os.O_WRONLY|os.O_APPEND, 0600)
	if err != nil {
		panic(err)
	}
	_, _ = f.WriteString(`{"id":"lixo", "op":"put", "chave":"kx",`)
	_ = f.Sync()
	_ = f.Close()

	// 8. Reinicia replica com o MESMO log: replay ignora a cauda.
	b2, err := novoNo(dir, "b", endA, 0)
	if err != nil {
		panic(err)
	}
	// Replica renascida escuta num endereco novo; primario continua no dele.
	lb, endB = ouvePorta()
	go b2.serve(lb)
	insiste(endB, 100)
	// O primario ainda aponta para a porta morta: atualiza o par.
	// (Descoberta automatica — gossip, failure detector — e trabalho futuro.)
	a.definePar(endB)
	checa(chama(endB, Pedido{ID: "g5", Op: "get", Chave: "k1"}).Result == "10", "replay trouxe k1")
	checa(chama(endB, Pedido{ID: "g6", Op: "get", Chave: "k3"}).Result == "", "k3 ausente antes do reenvio")

	// 9. Reenvio com o MESMO id converge sem duplicar.
	r = chama(endB, Pedido{ID: "op3", Op: "put", Chave: "k3", Valor: "30"})
	checa(r.OK, "reenvio aceito")
	checa(chama(endA, Pedido{ID: "g7", Op: "get", Chave: "k3"}).Result == "30", "k3 nos dois")
	checa(chama(endB, Pedido{ID: "g8", Op: "get", Chave: "k3"}).Result == "30", "k3 nos dois")

	// 10. Estado final igual.
	for _, kv := range [][2]string{{"k1", "10"}, {"k2", "20"}, {"k3", "30"}} {
		ra := chama(endA, Pedido{ID: "fa-" + kv[0], Op: "get", Chave: kv[0]}).Result
		rb := chama(endB, Pedido{ID: "fb-" + kv[0], Op: "get", Chave: kv[0]}).Result
		checa(ra == kv[1] && rb == kv[1], "igualdade final "+kv[0])
	}

	// 11. Par lento: 300 ms de atraso no encaminhamento, cliente nao trava.
	a.atraso.Store(int64(300 * time.Millisecond))
	t0 := time.Now()
	r = chama(endA, Pedido{ID: "op4", Op: "put", Chave: "k4", Valor: "40"})
	dt = time.Since(t0)
	checa(r.OK, "escrita com par lento responde ok")
	fmt.Printf("atraso ok: resposta em %s (lento, mas sem travar)\n", dt.Round(time.Millisecond))
	a.atraso.Store(0)
	checa(chama(endB, Pedido{ID: "g9", Op: "get", Chave: "k4"}).Result == "40", "k4 replicou apesar do atraso")
	fmt.Println("DISTRIBUIDO_OK")
	a.para()
	b2.para()
}
