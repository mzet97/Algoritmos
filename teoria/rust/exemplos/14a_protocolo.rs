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
