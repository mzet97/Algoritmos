---
layout: default
---

# Aula 14 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../14.md#15-exercícios-obrigatórios). Projeto Cargo (`cargo new aula14`, `cargo add tokio --features full`):

```toml
# Cargo.toml (trecho)
[dependencies]
tokio = { version = "1", features = ["full"] }
```

Exercicios 1, 2, 4, 6 e 7 em `src/main.rs` (simulados, sem rede):

```rust
use std::collections::{HashSet, VecDeque};
use std::sync::atomic::{AtomicUsize, Ordering};
use std::sync::Arc;
use std::time::{Duration, Instant};
use tokio::sync::{mpsc, Semaphore};
use tokio::task::JoinSet;

async fn baixar_simulado(id: usize, ms: u64) -> String {
    tokio::time::sleep(Duration::from_millis(ms)).await;
    format!("pagina-{}", id)
}

async fn fetch_urls(urls: Vec<&str>) -> Vec<String> {
    let mut conjunto = JoinSet::new();
    for (i, _url) in urls.iter().enumerate() {
        conjunto.spawn(baixar_simulado(i, 50));
    }
    let mut saida = Vec::new();
    while let Some(resultado) = conjunto.join_next().await {
        saida.push(resultado.unwrap());
    }
    saida.sort();
    saida
}

#[tokio::main]
async fn main() {
    // Exercicio 1: concorrente leva ~o maior tempo, nao a soma
    let inicio = Instant::now();
    let paginas = fetch_urls(vec!["a", "b", "c", "d"]).await;
    println!("fetch: {} paginas em {:?}", paginas.len(), inicio.elapsed());

    // Exercicio 2: pipeline gerador -> pares*2 -> soma
    let (tx1, mut rx1) = mpsc::channel::<i32>(8);
    let (tx2, mut rx2) = mpsc::channel::<i32>(8);
    tokio::spawn(async move {
        for n in 1..=100 {
            tx1.send(n).await.unwrap();
        }
    });
    tokio::spawn(async move {
        while let Some(n) = rx1.recv().await {
            if n % 2 == 0 {
                tx2.send(n * 2).await.unwrap();
            }
        }
    });
    let mut soma = 0;
    while let Some(n) = rx2.recv().await {
        soma += n;
    }
    println!("pipeline async soma: {}", soma);
    assert_eq!(soma, 5100);

    // Exercicio 4: no maximo 3 downloads simultaneos
    let semaforo = Arc::new(Semaphore::new(3));
    let simultaneos = Arc::new(AtomicUsize::new(0));
    let maximo = Arc::new(AtomicUsize::new(0));
    let mut conjunto = JoinSet::new();
    for id in 0..10 {
        let sem = Arc::clone(&semaforo);
        let sim = Arc::clone(&simultaneos);
        let max = Arc::clone(&maximo);
        conjunto.spawn(async move {
            let _permissao = sem.acquire().await.unwrap();
            let agora = sim.fetch_add(1, Ordering::SeqCst) + 1;
            max.fetch_max(agora, Ordering::SeqCst);
            baixar_simulado(id, 30).await;
            sim.fetch_sub(1, Ordering::SeqCst);
        });
    }
    while conjunto.join_next().await.is_some() {}
    println!("maximo simultaneo observado: {}", maximo.load(Ordering::SeqCst));
    assert!(maximo.load(Ordering::SeqCst) <= 3);

    // Exercicio 6: produtor espera quando o buffer (5) enche
    let (tx, mut rx) = mpsc::channel::<i32>(5);
    let produtor = tokio::spawn(async move {
        for i in 0..20 {
            tx.send(i).await.unwrap();
        }
    });
    let mut recebidos = 0;
    while let Some(_item) = rx.recv().await {
        tokio::time::sleep(Duration::from_millis(5)).await;
        recebidos += 1;
    }
    produtor.await.unwrap();
    println!("backpressure recebeu: {}", recebidos);
    assert_eq!(recebidos, 20);

    // Exercicio 7: crawler simulado com limite 4 e sem repeticao
    let semaforo = Arc::new(Semaphore::new(4));
    let mut fila: VecDeque<String> = VecDeque::new();
    let mut visitadas: HashSet<String> = HashSet::new();
    fila.push_back(String::from("http://exemplo.dev/"));
    let mut visitas = 0;
    while let Some(url) = fila.pop_front() {
        if visitas >= 50 || !visitadas.insert(url.clone()) {
            continue;
        }
        visitas += 1;
        let _permissao = semaforo.clone().acquire_owned().await.unwrap();
        baixar_simulado(visitas, 1).await;
        for vizinho in 0..2 {
            fila.push_back(format!("{}filho{}", url, vizinho));
        }
    }
    println!("crawler visitou: {}", visitas);
    assert_eq!(visitas, 50);
}
```

Exercicio 3, echo com timeout de 30s por conexao (`src/bin/echo.rs`):

```rust
use std::time::Duration;
use tokio::io::{AsyncBufReadExt, AsyncWriteExt, BufReader};
use tokio::net::TcpListener;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let listener = TcpListener::bind("127.0.0.1:7878").await?;
    println!("echo em 127.0.0.1:7878");
    loop {
        let (socket, _) = listener.accept().await?;
        tokio::spawn(async move {
            let (leitor, mut escritor) = socket.into_split();
            let mut linhas = BufReader::new(leitor).lines();
            loop {
                let proxima = tokio::time::timeout(Duration::from_secs(30), linhas.next_line());
                match proxima.await {
                    Ok(Ok(Some(linha))) => {
                        if escritor.write_all(linha.as_bytes()).await.is_err() {
                            return;
                        }
                        if escritor.write_all(b"\n").await.is_err() {
                            return;
                        }
                    }
                    _ => return, // timeout, erro ou fim da conexao
                }
            }
        });
    }
}
```

Exercicio 5, chat com salas (`src/bin/chat.rs`, recorte central):

```rust
use std::collections::HashMap;
use std::sync::Arc;
use tokio::io::{AsyncBufReadExt, AsyncWriteExt, BufReader};
use tokio::net::TcpListener;
use tokio::sync::{broadcast, mpsc, Mutex};

type Salas = Arc<Mutex<HashMap<String, broadcast::Sender<String>>>>;

async fn sala(salas: &Salas, nome: &str) -> broadcast::Sender<String> {
    let mut mapa = salas.lock().await;
    mapa.entry(nome.to_string())
        .or_insert_with(|| broadcast::channel(64).0)
        .clone()
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let salas: Salas = Arc::new(Mutex::new(HashMap::new()));
    let listener = TcpListener::bind("127.0.0.1:7879").await?;
    println!("chat em 127.0.0.1:7879 (comece em 'geral'; /join NOME troca)");
    loop {
        let (socket, endereco) = listener.accept().await?;
        let salas_cliente = Arc::clone(&salas);
        tokio::spawn(async move {
            let (leitor, mut escritor) = socket.into_split();
            // Tarefa leitora: transforma linhas do socket em eventos.
            let (tx_eventos, mut rx_eventos) = mpsc::channel::<String>(64);
            tokio::spawn(async move {
                let mut linhas = BufReader::new(leitor).lines();
                while let Ok(Some(linha)) = linhas.next_line().await {
                    if tx_eventos.send(linha).await.is_err() {
                        break;
                    }
                }
            });

            let mut atual = sala(&salas_cliente, "geral").await;
            let mut receptor = atual.subscribe();
            loop {
                tokio::select! {
                    evento = rx_eventos.recv() => {
                        match evento {
                            Some(texto) => {
                                if let Some(nova) = texto.strip_prefix("/join ") {
                                    atual = sala(&salas_cliente, nova.trim()).await;
                                    receptor = atual.subscribe();
                                } else {
                                    let _ = atual.send(format!("{}: {}", endereco, texto));
                                }
                            }
                            None => return,
                        }
                    }
                    mensagem = receptor.recv() => {
                        match mensagem {
                            Ok(m) => {
                                if escritor.write_all(m.as_bytes()).await.is_err() {
                                    return;
                                }
                                if escritor.write_all(b"\n").await.is_err() {
                                    return;
                                }
                            }
                            Err(_) => {
                                receptor = atual.subscribe();
                            }
                        }
                    }
                }
            }
        });
    }
}
```

Verificado nesta rodada: `cargo run` (fetch concorrente em ~50 ms, pipeline somando 5100, maximo simultaneo ≤ 3, 20 itens com backpressure, crawler parando em 50), echo respondendo linha a linha e chat com dois clientes trocando de sala e recebendo so a propria sala. O timeout de 30s do echo e a constante `Duration::from_secs(30)` no `select!` com `timeout` — para demonstrar sem esperar meia hora, reduza a constante num laboratorio seu e observe o fechamento.

**Autoverificacao:** pipeline fechando sozinho ao fim do gerador, semaforo nunca acima de 3 no contador e um cliente de chat que **nao** recebe mensagens da outra sala.
