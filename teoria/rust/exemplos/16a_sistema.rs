use std::fs;
use std::io::Read;
use std::process::Command;
use std::time::Instant;

/// Conta regioes e executaveis em /proc/self/maps (Linux).
/// Fora do Linux, devolve zeros e diz por que.
fn resumir_mapas() -> (usize, usize, &'static str) {
    let texto = fs::read_to_string("/proc/self/maps");
    let texto = match texto {
        Ok(t) => t,
        Err(_) => return (0, 0, "indisponivel fora do Linux"),
    };
    let mut total = 0;
    let mut exec = 0;
    for linha in texto.lines() {
        if linha.is_empty() {
            continue;
        }
        total += 1;
        if linha.contains("r-xp") {
            exec += 1;
        }
    }
    (total, exec, "/proc/self/maps")
}

/// Extrai VmRSS em KiB de /proc/self/status (Linux).
fn rss_kb() -> Option<u64> {
    let texto = fs::read_to_string("/proc/self/status").ok()?;
    for linha in texto.lines() {
        if let Some(resto) = linha.strip_prefix("VmRSS:") {
            return resto.split_whitespace().next()?.parse().ok();
        }
    }
    None
}

/// Regua honesta de syscall: read em /dev/null, N repeticoes.
/// Nota: read devolve Ok(0) (EOF imediato) — e o `read` (nao `read_exact`),
/// pois `read_exact` exigiria 1 byte e falharia com UnexpectedEof.
fn regua_read(repeticoes: u32) -> f64 {
    let mut f = fs::File::open("/dev/null").expect("/dev/null deve abrir");
    let mut b = [0u8; 1];
    let inicio = Instant::now();
    for _ in 0..repeticoes {
        // /dev/null sempre devolve Ok(0): conta a syscall, nao bytes.
        // O assert consome o retorno e cala o clippy::unused_io_amount com razao.
        let n = f.read(&mut b).expect("read em /dev/null nao falha");
        assert_eq!(n, 0, "dev/null mudou de comportamento?");
    }
    inicio.elapsed().as_secs_f64() / f64::from(repeticoes)
}

fn main() {
    let cpus = std::thread::available_parallelism()
        .map(|n| n.get())
        .unwrap_or(1);
    println!("cpus(paralelismo)={cpus} pid={}", std::process::id());

    // 1. Processo filho com codigo de saida conhecido.
    let estado = Command::new("/bin/true")
        .status()
        .expect("falha ao criar /bin/true (fora do Linux, ajuste)");
    assert!(estado.success());
    println!("filho /bin/true: ok");

    // 2. Falha contida: comando inexistente e argumento invalido.
    let ruim = Command::new("/nao/existe/de/jeito/nenhum").output();
    assert!(ruim.is_err(), "comando inexistente deveria falhar no spawn");
    println!("exec falho contido: {}", ruim.unwrap_err());

    // 3. Regua de syscall.
    let rep = 200_000u32;
    let t = regua_read(rep);
    println!("read(/dev/null): {:.1} ns/chamada ({rep} repeticoes)", t * 1e9);

    // 4. Mapa de memoria do proprio processo.
    let (total, exec, origem) = resumir_mapas();
    println!("regioes={total} executaveis={exec} origem={origem}");
    if origem == "/proc/self/maps" {
        assert!(total > 0 && exec > 0, "mapa vazio: invariante violada");
    }

    // 5. RSS antes e depois de 8 MiB tocados pagina a pagina.
    let antes = rss_kb();
    let mut bloco = vec![0u8; 8 << 20];
    for i in (0..bloco.len()).step_by(4096) {
        bloco[i] = 1; // uma pagina nova por iteracao
    }
    // Sem esta ancora, o otimizador apaga alocacao e toques: o bloco nao
    // e lido e o drop e livre. black_box torna a residencia observavel.
    std::hint::black_box(&bloco);
    let depois = rss_kb();
    println!("rss_kb: {antes:?} -> {depois:?}");
    if let (Some(a), Some(d)) = (antes, depois) {
        assert!(d > a, "RSS deveria crescer com 8 MiB tocados");
    }
    drop(bloco);

    // 6. Localidade: sequencial contra passo 8 (uma palavra por linha de 64 B).
    // Trabalho igualado: 8 passadas de N/8 somas = N somas por repeticao.
    let grande: Vec<u64> = (0..2_000_000u64).collect();
    let n = grande.len();
    let rep = 10;
    let inicio = Instant::now();
    let mut soma = 0u64;
    for _ in 0..rep {
        for &v in &grande {
            soma = soma.wrapping_add(v);
        }
    }
    let t_seq = inicio.elapsed().as_secs_f64() / f64::from(rep);
    let inicio = Instant::now();
    let mut soma2 = 0u64;
    for _ in 0..rep {
        for _passada in 0..8 {
            let mut i = 0;
            while i < n {
                soma2 = soma2.wrapping_add(grande[i]);
                i += 8;
            }
        }
    }
    let t_salto = inicio.elapsed().as_secs_f64() / f64::from(rep);
    println!("soma_seq: {t_seq:.6}s/media soma={soma}");
    println!("soma_salto8: {t_salto:.6}s/media soma={soma2} razao={:.2}x", t_salto / t_seq);
}
