---
layout: default
---

# Aula 13 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../13.md#15-exercícios-obrigatórios).

```rust
use std::collections::HashMap;
use std::sync::atomic::{AtomicI32, Ordering};
use std::sync::{mpsc, Arc, Mutex, RwLock};
use std::thread;
use std::time::{Duration, Instant};

fn eh_primo(n: i32) -> bool {
    if n < 2 {
        return false;
    }
    let mut d = 2;
    while d * d <= n {
        if n % d == 0 {
            return false;
        }
        d += 1;
    }
    true
}

fn main() {
    // Exercicio 1: soma paralela em 4 partes
    let dados: Vec<i32> = (0..1_000_000).collect();
    let esperado: i64 = dados.iter().map(|x| *x as i64).sum();
    let arc = Arc::new(dados);
    let mut handles = Vec::new();
    for parte in 0..4 {
        let clonado = Arc::clone(&arc);
        handles.push(thread::spawn(move || {
            clonado[parte * 250_000..(parte + 1) * 250_000]
                .iter()
                .map(|x| *x as i64)
                .sum::<i64>()
        }));
    }
    let paralelo: i64 = handles.into_iter().map(|h| h.join().unwrap()).sum();
    println!("soma paralela confere: {}", paralelo == esperado);
    assert_eq!(paralelo, esperado);

    // Exercicio 2: pipeline gerador -> filtro de primos -> soma
    let (tx1, rx1) = mpsc::channel::<i32>();
    let (tx2, rx2) = mpsc::channel::<i32>();
    let gerador = thread::spawn(move || {
        for n in 1..=100 {
            tx1.send(n).unwrap();
        }
    });
    let filtro = thread::spawn(move || {
        for n in rx1 {
            if eh_primo(n) {
                tx2.send(n).unwrap();
            }
        }
    });
    let mut soma_primos = 0;
    for primo in rx2 {
        soma_primos += primo;
    }
    gerador.join().unwrap();
    filtro.join().unwrap();
    println!("soma dos primos ate 100: {}", soma_primos);
    assert_eq!(soma_primos, 1060);

    // Exercicio 3: contador de tres formas
    let via_mutex = Arc::new(Mutex::new(0i32));
    let mut hs = Vec::new();
    for _ in 0..10 {
        let c = Arc::clone(&via_mutex);
        hs.push(thread::spawn(move || {
            for _ in 0..1000 {
                *c.lock().unwrap() += 1;
            }
        }));
    }
    for h in hs { h.join().unwrap(); }

    let via_atomico = Arc::new(AtomicI32::new(0));
    let mut hs = Vec::new();
    for _ in 0..10 {
        let c = Arc::clone(&via_atomico);
        hs.push(thread::spawn(move || {
            for _ in 0..1000 {
                c.fetch_add(1, Ordering::SeqCst);
            }
        }));
    }
    for h in hs { h.join().unwrap(); }

    let (tx, rx) = mpsc::channel::<i32>();
    for _ in 0..10 {
        let t = tx.clone();
        thread::spawn(move || {
            t.send(1000).unwrap();
        });
    }
    drop(tx);
    let via_canal: i32 = rx.iter().sum();
    println!("contadores: mutex={} atomico={} canal={}",
             *via_mutex.lock().unwrap(), via_atomico.load(Ordering::SeqCst), via_canal);
    assert_eq!(*via_mutex.lock().unwrap(), 10000);
    assert_eq!(via_atomico.load(Ordering::SeqCst), 10000);
    assert_eq!(via_canal, 10000);

    // Exercicio 4: buffer limitado bloqueia o produtor
    let (txl, rxl) = mpsc::sync_channel::<i32>(2);
    let produtor = thread::spawn(move || {
        for i in 0..10 {
            txl.send(i).unwrap();
        }
    });
    let mut recebidos = 0;
    for _item in rxl {
        thread::sleep(Duration::from_millis(5));
        recebidos += 1;
    }
    produtor.join().unwrap();
    println!("limitado recebeu: {}", recebidos);
    assert_eq!(recebidos, 10);

    // Exercicio 5: MapReduce de palavras
    let texto = "o rato roeu o queijo o rato fugiu o gato viu o rato";
    let palavras_todas: Vec<&str> = texto.split_whitespace().collect();
    let textos: Vec<String> = palavras_todas
        .chunks(4)
        .map(|c| c.join(" "))
        .collect();
    let mut hs = Vec::new();
    for pedaco in textos {
        hs.push(thread::spawn(move || {
            let mut mapa: HashMap<String, u32> = HashMap::new();
            for palavra in pedaco.split_whitespace() {
                *mapa.entry(palavra.to_string()).or_insert(0) += 1;
            }
            mapa
        }));
    }
    let mut total: HashMap<String, u32> = HashMap::new();
    for h in hs {
        for (palavra, n) in h.join().unwrap() {
            *total.entry(palavra).or_insert(0) += n;
        }
    }
    let mut ranking: Vec<(String, u32)> = total.into_iter().collect();
    ranking.sort_by(|a, b| b.1.cmp(&a.1).then_with(|| a.0.cmp(&b.0)));
    println!("top palavra: {:?}", ranking.first());
    assert_eq!(ranking.first().map(|(p, _)| p.as_str()), Some("o"));

    // Exercicio 6: RwLock contra Mutex com leitores dominantes
    fn carga_rwlock() -> Duration {
        let dados = Arc::new(RwLock::new((0..1000).collect::<Vec<i32>>()));
        let inicio = Instant::now();
        let mut hs = Vec::new();
        for _ in 0..8 {
            let d = Arc::clone(&dados);
            hs.push(thread::spawn(move || {
                for _ in 0..1000 {
                    let leitura = d.read().unwrap();
                    std::hint::black_box(leitura.len());
                }
            }));
        }
        for _ in 0..2 {
            let d = Arc::clone(&dados);
            hs.push(thread::spawn(move || {
                for i in 0..100 {
                    d.write().unwrap()[0] = i;
                }
            }));
        }
        for h in hs { h.join().unwrap(); }
        inicio.elapsed()
    }
    fn carga_mutex() -> Duration {
        let dados = Arc::new(Mutex::new((0..1000).collect::<Vec<i32>>()));
        let inicio = Instant::now();
        let mut hs = Vec::new();
        for _ in 0..8 {
            let d = Arc::clone(&dados);
            hs.push(thread::spawn(move || {
                for _ in 0..1000 {
                    let leitura = d.lock().unwrap();
                    std::hint::black_box(leitura.len());
                }
            }));
        }
        for _ in 0..2 {
            let d = Arc::clone(&dados);
            hs.push(thread::spawn(move || {
                for i in 0..100 {
                    d.lock().unwrap()[0] = i;
                }
            }));
        }
        for h in hs { h.join().unwrap(); }
        inicio.elapsed()
    }
    println!("rwlock={:?} mutex={:?} (tempos desta maquina)", carga_rwlock(), carga_mutex());

    println!("pool fica como desafio: ver texto");
}
```

3. Corretude igual, custos diferentes: `Mutex` serializa cada incremento; o atomico dispensa a trava mas so protege **uma** variavel; o canal elimina estado compartilhado e soma parciais no fim — o preco e nao ter leitura intermediaria do total. Medir antes de escolher: com trava por incremento, as tres formas aqui sao dominadas pelo custo de sincronizacao, nao pela aritmetica.

7. Thread pool com work stealing e o desafio aberto desta solucao: um pool com `Arc<Mutex<mpsc::Receiver>>` funciona, mas todas as threads disputam o mesmo mutex da fila — roubo de verdade pede filas por thread (como o crossbeam-deque faz). Entregue o pool simples primeiro, meça a contencao e so entao desenhe o roubo; declarar essa fronteira e parte da resposta.

**Autoverificacao:** soma paralela igual a sequencial, primos somando 1060, tres contadores em 10000 e o ranking do MapReduce deterministico pelo desempate.
