use std::sync::atomic::{AtomicU64, Ordering};
use std::time::Instant;

const N: usize = 20_000_000;
const REP: usize = 5;

/// Soma sequencial: baseline legivel e correto.
fn soma_seq(v: &[u64]) -> u64 {
    v.iter().sum()
}

/// Soma particionada com scopes: o borrow checker garante o join.
fn soma_par(v: &[u64], fatias: usize) -> u64 {
    let n = v.len() / fatias;
    let mut parcial = vec![0u64; fatias];
    std::thread::scope(|s| {
        for (w, p) in parcial.iter_mut().enumerate() {
            let ini = n * w;
            let fim = if w + 1 == fatias { v.len() } else { n * (w + 1) };
            s.spawn(move || {
                *p = v[ini..fim].iter().sum();
            });
        }
    });
    parcial.iter().sum()
}

/// Soma com atomico compartilhado: correta e serializada pela cache.
fn soma_contendida(v: &[u64], fatias: usize) -> u64 {
    let total = AtomicU64::new(0);
    // Emprestimo compartilhado: `&AtomicU64` e `Copy`, entao cada
    // closure `move` carrega so uma copia da referencia — sem mover o dono.
    let total = &total;
    std::thread::scope(|s| {
        for w in 0..fatias {
            let ini = (v.len() * w) / fatias;
            let fim = (v.len() * (w + 1)) / fatias;
            // `move`: a closure precisa ser dona de ini/fim (`v` e o
            // `&AtomicU64` sao Copy, entao mover so copia referencias).
            s.spawn(move || {
                let mut local = 0u64;
                for &x in &v[ini..fim] {
                    // Soma LOCAL por fatia, publica uma vez: contencao minima.
                    local = local.wrapping_add(x);
                }
                total.fetch_add(local, Ordering::Relaxed);
            });
        }
    });
    total.load(Ordering::Relaxed)
}

fn medir(repeticoes: usize, mut f: impl FnMut() -> u64) -> (u64, f64) {
    let mut melhor = f64::INFINITY;
    let mut resultado = 0;
    for _ in 0..repeticoes {
        let inicio = Instant::now();
        resultado = f();
        // black_box: consome o resultado de forma opaca para o compilador.
        // Sem isso, o otimizador prova que as repeticoes dao o mesmo valor
        // e mede zero — benchmark do otimizador, nao do programa.
        std::hint::black_box(resultado);
        let dt = inicio.elapsed().as_secs_f64();
        if dt < melhor {
            melhor = dt;
        }
    }
    (resultado, melhor)
}

fn main() {
    let v: Vec<u64> = (0..N as u64).map(|i| i % 97).collect();
    let esperado: u64 = v.iter().sum();
    assert!(esperado > 0);

    // Aquecimento fora da conta.
    let _ = soma_par(&v, 2);

    let (tot, t_seq) = medir(REP, || soma_seq(&v));
    assert_eq!(tot, esperado, "baseline incorreto");
    println!("seq: {t_seq:.3}s");

    for &w in &[2usize, 4, 8] {
        let (tot, t) = medir(REP, || soma_par(&v, w));
        assert_eq!(tot, esperado, "variante incorreta");
        println!("par{w}: {t:.3}s speedup={:.2}x", t_seq / t);
    }

    let (tot, t_con) = medir(REP, || soma_contendida(&v, 4));
    assert_eq!(tot, esperado, "contendido incorreto");
    println!("contendido4: {t_con:.3}s (correto, granularidade grossa)");

    // Conta do acelerador: aritmetica deterministica, sem hardware.
    let bytes = (N * 8) as f64;
    let banda_gbs = 16.0;
    let t_kernel = 0.001;
    let t_viagem = 2.0 * (bytes / (banda_gbs * 1024.0 * 1024.0 * 1024.0));
    println!("viagem={t_viagem:.4}s kernel={t_kernel:.3}s total={:.4}", t_viagem + t_kernel);
    if t_viagem + t_kernel < t_seq {
        println!("veredito: a GPU hipotetica venceria NESTE cenario");
    } else {
        println!("veredito: fique na CPU; a viagem custa mais que o calculo");
    }
    println!("hpc ok: baseline verificado, escala medida, conta feita");
}
