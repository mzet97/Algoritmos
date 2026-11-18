---
layout: default
---

# Aula 14a — TCP é fluxo de bytes: framing, concorrência e limites

---

## Objetivo da Aula

Construir um serviço TCP com enquadramento, leitura parcial e limites explícitos — e prová-lo contra fragmentação, coalescência, entrada inválida e cliente lento. Pré-requisitos: aulas 01 a 14, incluindo a ponte 12a. Não precisa saber frameworks HTTP/gRPC, `async`, `unsafe` nem FFI.

Ao terminar, você deverá explicar por que um `read` nunca corresponde a "uma mensagem", implementar framing com orçamento de bytes e tempo, isolar clientes lentos sem travar os demais e argumentar o que acontece quando cada limite estoura.

Esta aula é o ponto de partida do módulo R5: `async` não implica paralelismo, e nenhum framework dispensa o entendimento do fluxo de bytes que ele esconde.

Ao final, você deverá: explicar por que TCP é fluxo de bytes (não mensagens), implementar framing com prefixo de tamanho, isolar cliente lento sem travar os demais, e estimar o teto de conexões pelo custo de thread.
Mediremos com níveis de proficiência:
- **Nível 1 — Recordar:** `definir fluxo de bytes, framing, e o custo por conexão thread-bloqueante`;
- **Nível 2 — Aplicar:** `diagnosticar leitura parcial tratada como mensagem completa e corrigir com framing`;
- **Nível 3 — Projetar:** `projetar um protocolo com framing justificando limites e isolamento`.

---

## 1. TCP entrega bytes, não mensagens

`TcpStream::read` devolve *os bytes disponíveis até agora*: pode ser meia linha, três linhas, ou zero em timeout. Quem assume "um `read` = uma mensagem" escreve um programa que passa no teste local e quebra na primeira rede real. O enquadramento (framing) é contrato *seu*: aqui, linhas terminadas em `\n`, com teto de 4 KiB por mensagem e 32 conexões simultâneas.

## 2. Limites são o programa

Sem limites, uma entrada de 10 GB vira alocação de 10 GB; sem timeout, um cliente que conecta e cala ocupa uma thread para sempre. Cada limite precisa de um comportamento definido ao estourar:

- mensagem acima de 4 KiB → erro protocolado e fechamento;
- cliente mudo por mais de 2 s → timeout, contador de lentos incrementado;
- 33ª conexão simultânea → recusa imediata, sem enfileirar trabalho invisível.

## 3. Concorrência honesta com `std`

Cada conexão ganha uma thread (`thread::spawn` + `move` do `TcpStream`); um `Mutex<Estado>` guarda contadores; um `AtomicBool` sinaliza desligamento e cada `join` colhe sua thread. Threads do SO, não green threads: o custo por conexão é real e é por isso que o teto de 32 existe. `async` viria depois como otimização, não como correção.

## 4. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/14a_protocolo.rs) contém exatamente o programa abaixo. Compile direto com `rustc`, sem `cargo`:

```bash
rustc --edition 2024 teoria/rust/exemplos/14a_protocolo.rs -o proto14a
./proto14a
```

```rust
use std::io::{BufRead, BufReader, Read, Write};
use std::net::{Shutdown, TcpListener, TcpStream};
use std::sync::atomic::{AtomicBool, AtomicU64, Ordering};
use std::sync::{Arc, Mutex};
use std::thread;
use std::time::Duration;

const LIMITE_LINHA: usize = 4096;
const MAX_CONEXOES: usize = 32;

/// Estado compartilhado: so atraves de Mutex; invariante checada nos testes.
#[derive(Default)]
struct Estado {
    mensagens: u64,
    lentos: u64,
    recusadas: u64,
}

struct Servidor {
    estado: Mutex<Estado>,
    ativas: AtomicU64,
    desligando: AtomicBool,
    total_bytes: AtomicU64,
}

impl Servidor {
    fn trata_conexao(&self, mut fluxo: TcpStream) {
        fluxo.set_read_timeout(Some(Duration::from_secs(2))).ok();
        let mut leitor = BufReader::new(fluxo.try_clone().expect("clone"));
        let mut linha = Vec::new();
        loop {
            linha.clear();
            match leitor.read_until(b'\n', &mut linha) {
                Ok(0) => return, // EOF limpo: cliente fechou
                Ok(_) => {}
                Err(_) => {
                    self.estado.lock().unwrap().lentos += 1;
                    return; // timeout ou erro: conta e encerra
                }
            }
            if linha.len() > LIMITE_LINHA {
                let _ = fluxo.write_all(b"ERRO mensagem grande demais\n");
                let _ = fluxo.shutdown(Shutdown::Both);
                return;
            }
            if !linha.ends_with(b"\n") {
                return; // EOF no meio da linha: entrada truncada, descarta
            }
            self.estado.lock().unwrap().mensagens += 1;
            self.total_bytes.fetch_add(linha.len() as u64, Ordering::Relaxed);
            if fluxo.write_all(&linha).is_err() {
                return;
            }
        }
    }
}

/// Roda o servidor em background sobre uma porta efemera; devolve base + handle.
fn sobe_servidor(srv: Arc<Servidor>) -> (String, thread::JoinHandle<()>) {
    let ouvinte = TcpListener::bind("127.0.0.1:0").expect("bind");
    let base = format!("127.0.0.1:{}", ouvinte.local_addr().unwrap().port());
    let t = thread::spawn(move || {
        ouvinte.set_nonblocking(true).expect("nonblocking");
        while !srv.desligando.load(Ordering::Relaxed) {
            match ouvinte.accept() {
                Ok((fluxo, _)) => {
                    if srv.ativas.fetch_add(1, Ordering::SeqCst) >= MAX_CONEXOES as u64 {
                        srv.ativas.fetch_sub(1, Ordering::SeqCst);
                        srv.estado.lock().unwrap().recusadas += 1;
                        drop(fluxo);
                        continue;
                    }
                    let s2 = Arc::clone(&srv);
                    thread::spawn(move || {
                        s2.trata_conexao(fluxo);
                        s2.ativas.fetch_sub(1, Ordering::SeqCst);
                    });
                }
                Err(_) => thread::sleep(Duration::from_millis(5)),
            }
        }
    });
    (base, t)
}

fn fala(base: &str, partes: &[&[u8]]) -> Vec<u8> {
    let mut f = TcpStream::connect(base).expect("connect");
    f.set_read_timeout(Some(Duration::from_secs(5))).expect("timeout");
    for p in partes {
        f.write_all(p).expect("write");
    }
    f.shutdown(Shutdown::Write).ok(); // meia-fechadura: o servidor ve EOF apos os bytes
    let mut fora = Vec::new();
    let mut buf = [0u8; 1024];
    loop {
        match f.read(&mut buf) {
            Ok(0) => break,
            Ok(n) => fora.extend_from_slice(&buf[..n]),
            Err(_) => break,
        }
    }
    fora
}

fn main() {
    let srv = Arc::new(Servidor {
        estado: Mutex::new(Estado::default()),
        ativas: AtomicU64::new(0),
        desligando: AtomicBool::new(false),
        total_bytes: AtomicU64::new(0),
    });
    let (base, aceita) = sobe_servidor(Arc::clone(&srv));

    // 1. Eco simples.
    assert_eq!(fala(&base, &[b"OLA\n"]), b"OLA\n");

    // 2. Fragmentado byte a byte: um read nunca e uma mensagem.
    let msg: Vec<u8> = b"FRAG".to_vec();
    let partes: Vec<&[u8]> = msg.iter().map(std::slice::from_ref).collect();
    let mut com_nl: Vec<Vec<u8>> = partes.into_iter().map(|s| s.to_vec()).collect();
    com_nl.push(b"\n".to_vec());
    let refs: Vec<&[u8]> = com_nl.iter().map(|v| v.as_slice()).collect();
    assert_eq!(fala(&base, &refs), b"FRAG\n");

    // 3. Coalescido: duas mensagens num so segmento.
    assert_eq!(fala(&base, &[b"A\nB\n"]), b"A\nB\n");

    // 4. Grande demais: erro protocolado, sem panico.
    let grande = vec![b'X'; LIMITE_LINHA + 10];
    let mut g = grande.clone();
    g.push(b'\n');
    let r = fala(&base, &[&g]);
    assert_eq!(r, b"ERRO mensagem grande demais\n");

    // 5. Truncado: EOF sem \n nao gera resposta.
    assert_eq!(fala(&base, &[b"SEM-FIM"]), b"");

    // 6. Cliente lento: conecta e cala; servidor conta e encerra (2 s).
    let antes = srv.estado.lock().unwrap().lentos;
    {
        let _muda = TcpStream::connect(&base).expect("connect lento");
        thread::sleep(Duration::from_millis(2600));
    }
    let depois = srv.estado.lock().unwrap().lentos;
    assert!(depois > antes, "cliente lento deveria ser contado");

    let e = srv.estado.lock().unwrap();
    println!("mensagens={} lentos={} recusadas={}", e.mensagens, depois, e.recusadas);
    assert!(e.mensagens >= 4, "eco, fragmentado, coalescido(2 msgs)");
    drop(e);
    srv.desligando.store(true, Ordering::Relaxed);
    aceita.join().expect("aceita");
    println!("protocolo ok: framing, limites e lentos verificados");
}
```

Saída esperada (porta efêmera varia):

```text
mensagens=N lentos=M recusadas=0
protocolo ok: framing, limites e lentos verificados
```

Leia com o modelo certo: o teste fragmentado prova que `read_until` junta o que o TCP dividiu; o teste truncado prova que EOF no meio da linha descarta sem responder; o cliente lento prova que timeout conta e encerra em vez de vazar thread. `N >= 4` porque o coalescido conta duas mensagens.

### O que a memória e as threads fazem aqui?

Cada conexão vive numa thread do SO com sua pilha; o `Arc<Servidor>` compartilha o estado com contagem de referências atômica. `Mutex<Estado>` serializa contadores; `total_bytes` usa `fetch_add` relaxado porque só a soma final importa, não a ordem. O teto de 32 conexões é honestidade arquitetural: threads custam ~2 MiB de pilha virtual cada — sem teto, cliente malicioso vira exaustão de memória.

## 5. Erros comuns e implicações defensivas

1. **Assumir mensagem por `read`:** o bug clássico de rede. Enquadramento sempre, com teto.
2. **Sem teto de bytes:** entrada gigante vira alocação gigante. Limite antes de acumular.
3. **Sem timeout:** cliente mudo ocupa thread para sempre. Timeout + contador + encerramento.
4. **Sem teto de conexões:** aceitar sem limite é exaustão sob demanda. Recuse com contador, não com fila invisível.
5. **Compartilhar sem sincronizar:** contadores fora do `Mutex`/atômico são data race. O borrow checker não enxerga entre threads sem o tipo certo (`Mutex`, `Arc`).
6. **`unwrap` em produção:** aqui marca invariante de teste; em serviço, erro vira métrica e fechamento limpo, nunca pânico por entrada externa.

## 6. Ponte com Python, Go e C++

Python: `socket` + threads conta a mesma história (aula 31 da trilha Python); Go: `net.Conn` com `SetDeadline` (aula 16/30); C++: `poll` + buffers por conexão (aula 31). O protocolo muda de roupa; o fluxo de bytes, os limites e o cliente lento são os mesmos em toda trilha.

## 7. Exercícios obrigatórios

1. Desenhe o caminho de `PING\n` enviado byte a byte até o eco, marcando cada `read` parcial e o ponto onde a linha completa.
2. Mude o teto para 64 bytes e preveja qual teste quebra primeiro. Confirme executando.
3. Remova o `set_read_timeout` e preveja o que acontece com o teste do cliente lento. Confirme com timeout externo.
4. Conecte 40 clientes de uma vez e verifique `recusadas > 0` sem travar os 32 atendidos.

Use [pistas graduais](exercicios/14a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/14a_solucoes.md) depois de registrar sua própria explicação.

## 8. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- explicar por que um `read` nunca equivale a uma mensagem;
- implementar framing com teto de bytes e de tempo e dizer o que cada estouro faz;
- isolar cliente lento sem travar os demais e provar com contador;
- argumentar o teto de conexões em custo de thread;
- rodar o exemplo e confrontar cada número com o contrato;
- apontar o que `Mutex`/`Arc`/atômicos garantem e o que não garantem.

**Próxima aula:** `async` e runtime. O raciocínio desta aula sustenta a motivação do assíncrono: o custo por conexão que o `async` tenta derrubar.

---

### Como esta aula será avaliada

| Critério | Peso |
| -------- | ---: |
| Conceitos e vocabulário (nível 1) | 20% |
| Previsão de comportamento e diagnóstico de erros (nível 2) | 40% |
| Projeto e justificativa de decisões (nível 3) | 30% |
| Registro de comandos, saídas e interpretação | 10% |

**Aprovação:** nota mínima 7,0; pelo menos 60% dos pontos devem vir de previsão e diagnóstico (níveis 2 e 3), não apenas de código que compila.

**Para se aprofundar:**

1. *The Rustonomicon* ([doc.rust-lang.org/nomicon](https://doc.rust-lang.org/nomicon)); 2. Documentação do Tokio ([tokio.rs](https://tokio.rs)); 3. *The Rust Performance Book* ([nnethercote.github.io/perf-book](https://nnethercote.github.io/perf-book)).
