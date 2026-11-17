// Aula 23b (Rust) — dois nos chave-valor com log duravel e idempotencia.
// Modos: `checar` (roteiro de falhas) | `no <porta> <log> <par> <atraso_ms>`
// Protocolo por linhas (loopback): PUT <id> <chave> <valor> [F] | GET <chave>
use std::collections::HashMap;
use std::io::{BufRead, BufReader, Read, Write};
use std::net::{TcpListener, TcpStream};
use std::process::{Child, Command, Stdio};
use std::sync::{Arc, Mutex};
use std::time::{Duration, Instant, SystemTime, UNIX_EPOCH};

const LINHA_MAX: usize = 512;

#[derive(Clone)]
struct Registro {
    id: String,
    chave: String,
    valor: String,
}

struct Estado {
    regs: Vec<Registro>,
    por_id: HashMap<String, String>,
    log: std::fs::File,
}

struct No {
    estado: Mutex<Estado>,
    par: u16,
    atraso_ms: u64,
}

fn token_ok(s: &str, max: usize) -> bool {
    !s.is_empty()
        && s.len() <= max
        && s.bytes()
            .all(|b| b.is_ascii_alphanumeric() || b == b'.' || b == b'_' || b == b'-')
}

fn replay(caminho: &str) -> (Vec<Registro>, HashMap<String, String>) {
    let mut regs = Vec::new();
    let mut por_id = HashMap::new();
    if let Ok(conteudo) = std::fs::read_to_string(caminho) {
        for linha in conteudo.lines() {
            let t: Vec<&str> = linha.split_whitespace().collect();
            if t.len() == 3 && token_ok(t[0], 31) && token_ok(t[1], 63) && token_ok(t[2], 127)
                && !por_id.contains_key(t[0])
            {
                por_id.insert(t[0].to_string(), t[2].to_string());
                regs.push(Registro {
                    id: t[0].into(),
                    chave: t[1].into(),
                    valor: t[2].into(),
                });
            } // linha malformada (cauda rasgada): ignorada, como na 23a de C
        }
    }
    (regs, por_id)
}

fn conectar(porta: u16, timeout: Duration) -> std::io::Result<TcpStream> {
    let s = TcpStream::connect_timeout(&format!("127.0.0.1:{porta}").parse().unwrap(), timeout)?;
    s.set_read_timeout(Some(Duration::from_secs(2)))?;
    s.set_write_timeout(Some(Duration::from_secs(2)))?;
    Ok(s)
}

fn enviar_linha(s: &mut TcpStream, linha: &str) -> std::io::Result<()> {
    s.write_all(linha.as_bytes())?;
    s.write_all(b"\n")
}

fn ler_linha(s: &TcpStream, buf: &mut String) -> std::io::Result<usize> {
    let mut leitor = BufReader::new(s);
    let mut limitada = leitor.by_ref().take((LINHA_MAX + 1) as u64);
    let mut linha = String::new();
    let n = limitada.read_line(&mut linha)?;
    if n > LINHA_MAX {
        return Err(std::io::Error::new(std::io::ErrorKind::InvalidData, "longa"));
    }
    buf.push_str(linha.trim_end());
    Ok(n)
}

impl No {
    fn encaminhar(&self, r: &Registro) {
        if self.par == 0 {
            return;
        }
        if self.atraso_ms > 0 {
            std::thread::sleep(Duration::from_millis(self.atraso_ms));
        }
        if let Ok(mut s) = conectar(self.par, Duration::from_millis(2000)) {
            let _ = enviar_linha(&mut s, &format!("PUT {} {} {} F", r.id, r.chave, r.valor));
            let mut resp = String::new();
            let _ = ler_linha(&s, &mut resp);
        } // par caido: degrada, nao trava
    }

    fn tratar(&self, linha: &str) -> String {
        let t: Vec<&str> = linha.split_whitespace().collect();
        if t.is_empty() {
            return "ERR vazio".into();
        }
        match t[0] {
            "GET" => {
                if t.len() != 2 || !token_ok(t[1], 63) {
                    return "ERR get_invalido".into();
                }
                let e = self.estado.lock().unwrap();
                for r in e.regs.iter().rev() {
                    if r.chave == t[1] {
                        return format!("OK {}", r.valor);
                    }
                }
                "NAO_ENCONTRADO".into()
            }
            "PUT" => {
                let encaminhado = t.len() == 5 && t[4] == "F";
                if (t.len() != 4 && !encaminhado)
                    || !token_ok(t[1], 31)
                    || !token_ok(t[2], 63)
                    || !token_ok(t[3], 127)
                {
                    return "ERR put_invalido".into();
                }
                let r = Registro {
                    id: t[1].into(),
                    chave: t[2].into(),
                    valor: t[3].into(),
                };
                let (resp, novo) = {
                    let mut e = self.estado.lock().unwrap();
                    if let Some(v) = e.por_id.get(&r.id) {
                        (format!("OK {v} REPETIDO"), false)
                    } else {
                        // persistir (sync_all) ANTES de aplicar e responder
                        let linha_log = format!("{} {} {}\n", r.id, r.chave, r.valor);
                        if e.log.write_all(linha_log.as_bytes()).is_err()
                            || e.log.sync_all().is_err()
                        {
                            return "ERR log".into();
                        }
                        e.por_id.insert(r.id.clone(), r.valor.clone());
                        e.regs.push(r.clone());
                        (format!("OK {}", r.valor), true)
                    }
                };
                if novo && !encaminhado {
                    self.encaminhar(&r); // fora do mutex, antes de responder
                }
                resp
            }
            _ => "ERR op_desconhecida".into(),
        }
    }
}

fn modo_no(porta: u16, caminho: &str, par: u16, atraso: u64) -> ! {
    let (regs, por_id) = replay(caminho);
    let log = std::fs::OpenOptions::new()
        .create(true)
        .append(true)
        .open(caminho)
        .expect("log");
    let no = Arc::new(No {
        estado: Mutex::new(Estado { regs, por_id, log }),
        par,
        atraso_ms: atraso,
    });
    let listener = TcpListener::bind(("127.0.0.1", porta)).expect("bind");
    for fluxo in listener.incoming() {
        if let Ok(mut s) = fluxo {
            let no = Arc::clone(&no);
            std::thread::spawn(move || {
                let _ = s.set_read_timeout(Some(Duration::from_secs(10)));
                loop {
                    let mut linha = String::new();
                    match ler_linha(&s, &mut linha) {
                        Ok(0) => break,
                        Ok(_) => {
                            let resp = no.tratar(&linha);
                            if enviar_linha(&mut s, &resp).is_err() {
                                break;
                            }
                        }
                        Err(_) => break, // truncado/longo: fecha so esta conexao
                    }
                }
            });
        }
    }
    unreachable!()
}

// ---------------- roteiro de falhas ----------------
fn porta_livre() -> u16 {
    TcpListener::bind(("127.0.0.1", 0))
        .unwrap()
        .local_addr()
        .unwrap()
        .port()
}

fn iniciar(exe: &str, porta: u16, log: &str, par: u16, atraso: u64) -> Child {
    let filho = Command::new(exe)
        .args(["no", &porta.to_string(), log, &par.to_string(), &atraso.to_string()])
        .stdout(Stdio::null())
        .stderr(Stdio::null())
        .spawn()
        .unwrap();
    for _ in 0..100 {
        if conectar(porta, Duration::from_millis(200)).is_ok() {
            return filho;
        }
        std::thread::sleep(Duration::from_millis(50));
    }
    panic!("no nao aceitou a tempo");
}

fn cliente(porta: u16, linha: &str) -> Option<String> {
    let mut s = conectar(porta, Duration::from_millis(2000)).ok()?;
    enviar_linha(&mut s, linha).ok()?;
    let mut resp = String::new();
    ler_linha(&s, &mut resp).ok()?;
    Some(resp)
}

fn checar() -> i32 {
    let exe = std::env::current_exe().unwrap();
    let exe = exe.to_str().unwrap().to_string();
    let selo = SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_nanos();
    let dir = std::env::temp_dir().join(format!("rust23b_{selo}"));
    std::fs::create_dir_all(&dir).unwrap();
    let log_a = dir.join("a.log").to_str().unwrap().to_string();
    let log_b = dir.join("b.log").to_str().unwrap().to_string();
    let (pa, pb) = (porta_livre(), porta_livre());
    let mut n = 0;
    let mut verifica = |cond: bool, msg: &str| {
        if !cond {
            eprintln!("FALHA: {msg}");
            std::process::exit(1);
        }
        n += 1;
        println!("ok {n} - {msg}");
    };

    let mut a = iniciar(&exe, pa, &log_a, pb, 0);
    let mut b = iniciar(&exe, pb, &log_b, pa, 0);
    verifica(cliente(pa, "PUT id1 k1 v1").as_deref() == Some("OK v1"), "PUT inicial OK");
    verifica(cliente(pb, "GET k1").as_deref() == Some("OK v1"), "replica recebe");
    verifica(
        cliente(pa, "PUT id1 k1 vDIF").as_deref() == Some("OK v1 REPETIDO"),
        "duplicata vale a primeira (REPETIDO)",
    );
    verifica(
        cliente(pa, "PUT ruim k1").map(|r| r.starts_with("ERR")) == Some(true),
        "malformado recebe ERR",
    );
    b.kill().unwrap();
    let _ = b.wait();
    verifica(
        cliente(pa, "PUT id2 k2 v2").as_deref() == Some("OK v2"),
        "escrita com replica caida (degradado)",
    );
    std::fs::write(&log_b, {
        let mut c = std::fs::read(&log_b).unwrap_or_default();
        c.extend_from_slice(b"idRasgado chaveRasgada");
        c
    })
    .unwrap();
    let mut b2 = iniciar(&exe, pb, &log_b, pa, 0);
    verifica(
        cliente(pb, "GET k1").as_deref() == Some("OK v1"),
        "replay ignora cauda rasgada",
    );
    verifica(
        cliente(pb, "GET k2").as_deref() == Some("NAO_ENCONTRADO"),
        "escrita da queda ausente na replica",
    );
    verifica(
        cliente(pb, "PUT id2 k2 v2 F").as_deref() == Some("OK v2"),
        "reenvio dirigido converge",
    );
    a.kill().unwrap();
    let _ = a.wait();
    let mut a2 = iniciar(&exe, pa, &log_a, pb, 400);
    let t0 = Instant::now();
    let r = cliente(pa, "PUT id3 k3 v3");
    let dt = t0.elapsed();
    verifica(
        r.as_deref() == Some("OK v3") && dt >= Duration::from_millis(350) && dt < Duration::from_secs(5),
        "atraso de 400 ms atrasa sem travar",
    );
    verifica(cliente(pb, "GET k3").as_deref() == Some("OK v3"), "atrasada chega a replica");
    a2.kill().unwrap();
    let _ = a2.wait();
    b2.kill().unwrap();
    let _ = b2.wait();
    println!("DISTRIBUIDO_OK checks={n}");
    0
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() >= 2 && args[1] == "no" {
        modo_no(
            args[2].parse().unwrap(),
            &args[3],
            args[4].parse().unwrap(),
            args[5].parse().unwrap(),
        );
    } else if args.len() == 1 || (args.len() == 2 && args[1] == "checar") {
        std::process::exit(checar());
    } else {
        eprintln!("uso: {} [checar]", args[0]);
        std::process::exit(2);
    }
}
