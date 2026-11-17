// Aula 23c — vetorizacao em Rust: o compilador vetoriza o laco que voce
// escreveu para ser vetorizavel; NUMA contado pela /sys, nao presumido.
use std::time::Instant;

fn lcg(estado: &mut u64) -> f32 {
    *estado = estado.wrapping_mul(6364136223846793005).wrapping_add(1);
    ((*estado >> 40) as f32) / 16_777_216.0
}

// Soma sequencial: uma cadeia de dependencia so.
fn soma_seq(v: &[f32]) -> f32 {
    let mut s = 0.0f32;
    for &x in v {
        s += x;
    }
    s
}

// Quatro acumuladores em chunks: quebra a cadeia e expoe vetorizacao.
// A ordem de soma muda -> o resultado muda nos ultimos bits (float).
fn soma_4(v: &[f32]) -> f32 {
    let mut acc = [0.0f32; 4];
    let chunks = v.chunks_exact(4);
    let resto = chunks.remainder();
    for c in chunks {
        acc[0] += c[0];
        acc[1] += c[1];
        acc[2] += c[2];
        acc[3] += c[3];
    }
    let mut s = (acc[0] + acc[1]) + (acc[2] + acc[3]);
    for &x in resto {
        s += x;
    }
    s
}

fn referencia(v: &[f32]) -> f64 {
    v.iter().map(|&x| x as f64).sum()
}

fn nos_numa() -> i32 {
    match std::fs::read_dir("/sys/devices/system/node") {
        Ok(entradas) => entradas
            .flatten()
            .filter(|e| {
                let nome = e.file_name();
                let nome = nome.to_string_lossy();
                nome.starts_with("node")
                    && nome[4..].chars().all(|c| c.is_ascii_digit())
                    && nome.len() > 4
            })
            .count() as i32,
        Err(_) => -1,
    }
}

fn main() {
    let n = 10_000_000usize;
    let mut estado = 9u64;
    let v: Vec<f32> = (0..n).map(|_| lcg(&mut estado)).collect();

    let ref64 = referencia(&v);
    let r_seq = soma_seq(&v) as f64;
    let r_4 = soma_4(&v) as f64;
    let e_seq = (r_seq - ref64).abs() / ref64;
    let e_4 = (r_4 - ref64).abs() / ref64;
    println!("erro vs double: seq={e_seq:.2e} 4acum={e_4:.2e}");
    assert!(e_seq < 2e-2 && e_4 < 2e-3 && e_4 < e_seq);

    let mede = |f: &dyn Fn(&[f32]) -> f32| {
        let t0 = Instant::now();
        let mut total = 0.0f32;
        for _ in 0..5 {
            total += f(&v);
        }
        std::hint::black_box(total);
        t0.elapsed().as_secs_f64() / 5.0
    };
    let t_seq = mede(&soma_seq);
    let t_4 = mede(&soma_4);
    println!(
        "seq={t_seq:.4}s 4acum={t_4:.4}s razao={:.2}x nos_numa={}",
        t_seq / t_4,
        nos_numa()
    );
    assert!(t_4 < t_seq);
    println!("23c ok: corretude por tolerancia, quatro acumuladores vencem e NUMA contado");
}
