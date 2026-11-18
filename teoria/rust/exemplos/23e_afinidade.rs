// Aula 23e Rust — Afinidade, primeiro toque e o laboratorio multino.
//
// Triade STREAM-like (a = b + s*c) sobre 64 MiB em dois modos: (a)
// primeiro toque — cada thread inicializa o pedaco que vai computar;
// (b) tardio — a main inicializa tudo e as threads so computam. O
// checksum prova que os dois modos calculam igual; o tempo e medida,
// nao veredito: nesta maquina (um no NUMA) o efeito esperado e ~zero,
// e dize-lo e o resultado honesto da aula. A afinidade entra por fora
// (`taskset`), lida de /proc/self/status.
//
// Compilar: rustc --edition 2024 -O -D warnings teoria/rust/exemplos/23e_afinidade.rs -o /tmp/23e
// Executar: /tmp/23e
// Executar preso a um nucleo: taskset -c 0 /tmp/23e
use std::time::Instant;

const N: usize = 16_777_216; // 64 MiB em f32
const S: f32 = 2.5;
const N_THREADS: usize = 8;

fn triade(a: &mut [f32], b: &[f32], c: &[f32]) {
    let tam = N.div_ceil(N_THREADS);
    std::thread::scope(|s| {
        for ((aa, bb), cc) in a
            .chunks_mut(tam)
            .zip(b.chunks(tam))
            .zip(c.chunks(tam))
        {
            s.spawn(move || {
                for i in 0..aa.len() {
                    aa[i] = bb[i] + S * cc[i];
                }
            });
        }
    });
}

fn inicializa(v: &mut [f32], base: f32) {
    let tam = N.div_ceil(N_THREADS);
    std::thread::scope(|s| {
        let mut ini = 0usize;
        for vv in v.chunks_mut(tam) {
            let base_local = base + ini as f32 * 0.5;
            ini += vv.len();
            s.spawn(move || {
                for (i, x) in vv.iter_mut().enumerate() {
                    *x = base_local + i as f32 * 0.5;
                }
            });
        }
    });
}

fn soma(v: &[f32]) -> f64 {
    v.iter().map(|&x| x as f64).sum()
}

fn cpus_permitidas() -> String {
    let status = std::fs::read_to_string("/proc/self/status").unwrap_or_default();
    status
        .lines()
        .find(|l| l.starts_with("Cpus_allowed_list:"))
        .unwrap_or("Cpus_allowed_list: ?")
        .to_string()
}

fn nos_numa() -> usize {
    std::fs::read_dir("/sys/devices/system/node")
        .map(|r| {
            r.filter_map(|e| e.ok())
                .filter(|e| {
                    e.file_name().to_string_lossy().starts_with("node")
                        && e.file_type().map(|t| t.is_dir()).unwrap_or(false)
                })
                .count()
        })
        .unwrap_or(0)
}

fn main() {
    println!("{}", cpus_permitidas());
    println!("nos_numa={} threads={N_THREADS} n={N}", nos_numa());

    // Modo (a): primeiro toque — quem computa, inicializa.
    let mut b = vec![0.0f32; N];
    let mut c = vec![0.0f32; N];
    inicializa(&mut b, 1.0);
    inicializa(&mut c, 3.0);
    let mut a = vec![0.0f32; N];
    let t0 = Instant::now();
    triade(&mut a, &b, &c);
    let t_toque = t0.elapsed();
    let soma_toque = soma(&a);

    // Modo (b): tardio — a main inicializa tudo sozinha.
    for (i, x) in b.iter_mut().enumerate() {
        *x = 1.0 + i as f32 * 0.5;
    }
    for (i, x) in c.iter_mut().enumerate() {
        *x = 3.0 + i as f32 * 0.5;
    }
    let t0 = Instant::now();
    triade(&mut a, &b, &c);
    let t_tardio = t0.elapsed();
    let soma_tardia = soma(&a);

    assert_eq!(soma_toque, soma_tardia, "modos divergem: bug, nao NUMA");
    let gb = (3 * N * 4) as f64 / 1e9;
    println!(
        "primeiro_toque={:.3}s ({:.2} GB/s) tardio={:.3}s ({:.2} GB/s) razao={:.2}x",
        t_toque.as_secs_f64(),
        gb / t_toque.as_secs_f64(),
        t_tardio.as_secs_f64(),
        gb / t_tardio.as_secs_f64(),
        t_tardio.as_secs_f64() / t_toque.as_secs_f64()
    );
    println!("23e ok: checksum igual nos dois modos, afinidade lida, 1 no = laboratorio multino declarado");
}
