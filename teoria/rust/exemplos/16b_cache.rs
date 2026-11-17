// Aula 16b — cache, localidade e custo de syscall em espaco de usuario.
use std::time::Instant;

fn main() {
    // Localidade: mesma soma, dois percursos. Por linhas atravessa a
    // matriz em ordem de memoria; por colunas salta uma linha por passo.
    let n = 2048usize;
    let mut m = vec![0u64; n * n];
    for (i, v) in m.iter_mut().enumerate() {
        *v = (i % 251) as u64;
    }
    let t0 = Instant::now();
    let mut soma_linhas = 0u64;
    for i in 0..n {
        for j in 0..n {
            soma_linhas += m[i * n + j];
        }
    }
    let t_linhas = t0.elapsed();
    let t0 = Instant::now();
    let mut soma_colunas = 0u64;
    for j in 0..n {
        let mut parcial = 0u64;
        for i in 0..n {
            parcial += m[i * n + j];
        }
        // black_box por coluna: impede o compilador de transformar o
        // percurso por colunas em percurso por linhas (soma inteira e
        // associativa e o otimizador sabe disso).
        soma_colunas += std::hint::black_box(parcial);
    }
    let t_colunas = t0.elapsed();
    assert_eq!(soma_linhas, soma_colunas);
    println!(
        "linhas={t_linhas:?} colunas={t_colunas:?} razao={:.2}x",
        t_colunas.as_secs_f64() / t_linhas.as_secs_f64()
    );
    assert!(t_colunas > t_linhas);

    // Custo de syscall: ler 1 byte de /dev/zero 200 mil vezes via std.
    // Cada read atravessa a fronteira usuario/kernel; comparar com o
    // custo de tocar memoria ja mapeada mostra onde o tempo mora.
    use std::io::Read;
    let mut dev = std::fs::File::open("/dev/zero").unwrap();
    let mut buf = [0u8; 1];
    let t0 = Instant::now();
    for _ in 0..200_000 {
        dev.read_exact(&mut buf).unwrap();
    }
    let t_sys = t0.elapsed();
    let t0 = Instant::now();
    let mut acc = 0u64;
    for i in 0..200_000u64 {
        acc += m[(i as usize) % (n * n)];
    }
    let t_mem = t0.elapsed();
    std::hint::black_box(acc);
    println!(
        "200k reads de 1 byte={t_sys:?} vs 200k acessos a memoria={t_mem:?} (razao {:.1}x)",
        t_sys.as_secs_f64() / t_mem.as_secs_f64()
    );
    assert!(t_sys > t_mem * 5);

    // Pagina da maquina (o que a 10a de C mediu, agora por /proc).
    let maps = std::fs::read_to_string("/proc/self/statm").unwrap();
    let paginas: u64 = maps.split_whitespace().next().unwrap().parse().unwrap();
    println!("statm: tamanho virtual do processo = {paginas} paginas");
    assert!(paginas > 0);
    println!("16b ok: localidade medida, syscall cronometrado, pagina lida do /proc");
}
