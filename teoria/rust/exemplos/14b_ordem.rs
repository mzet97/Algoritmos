// Aula 14b — ordem de memoria, backpressure e cancelamento cooperativo.
use std::sync::atomic::{AtomicBool, AtomicU64, Ordering};
use std::sync::mpsc::sync_channel;
use std::time::{Duration, Instant};

static DADO: AtomicU64 = AtomicU64::new(0);
static PRONTO: AtomicBool = AtomicBool::new(false);

fn main() {
    // Mensagem por atomicos: quem escreve DADO com Release e sinaliza,
    // quem le com Acquire e ve o sinal, ve tambem o dado. 10 mil voltas.
    for volta in 0..10_000u64 {
        PRONTO.store(false, Ordering::Relaxed);
        let t = std::thread::spawn(move || {
            while !PRONTO.load(Ordering::Acquire) {
                std::hint::spin_loop();
            }
            DADO.load(Ordering::Relaxed)
        });
        DADO.store(volta + 1000, Ordering::Relaxed);
        PRONTO.store(true, Ordering::Release);
        assert_eq!(t.join().unwrap(), volta + 1000);
    }
    println!("mensagem Release/Acquire ok (10000 voltas)");

    // Backpressure: canal limitado a 4. Com o consumidor dormindo, o
    // produtor bloqueia na 5a mensagem — fila infinita esconderia isso.
    let (tx, rx) = sync_channel::<u32>(4);
    let consumidor = std::thread::spawn(move || {
        std::thread::sleep(Duration::from_millis(300));
        let mut recebidos = 0;
        while rx.recv().is_ok() {
            recebidos += 1;
        }
        recebidos
    });
    let t0 = Instant::now();
    for i in 0..5 {
        tx.send(i).unwrap();
    }
    let bloqueio = t0.elapsed();
    drop(tx);
    assert_eq!(consumidor.join().unwrap(), 5);
    println!("backpressure: 5o envio esperou {bloqueio:?} (canal de 4, consumidor dormindo)");
    assert!(bloqueio >= Duration::from_millis(200));

    // Cancelamento cooperativo: worker checa a flag e para sem ser morto.
    static PARAR: AtomicBool = AtomicBool::new(false);
    let worker = std::thread::spawn(|| {
        let mut voltas = 0u64;
        while !PARAR.load(Ordering::Relaxed) {
            voltas += 1;
            std::hint::spin_loop();
        }
        voltas
    });
    std::thread::sleep(Duration::from_millis(50));
    PARAR.store(true, Ordering::Release);
    let voltas = worker.join().unwrap();
    assert!(voltas > 0);
    println!("cancelamento cooperativo ok ({voltas} voltas ate ver a flag)");
    println!("14b ok: ordem, backpressure e cancelamento demonstrados");
}
