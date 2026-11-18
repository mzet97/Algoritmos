---
layout: default
---

# Aula 17 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../17.md#14-exercícios-obrigatórios). Ambiente declarado: `criterion`, `cargo flamegraph` e `cargo asm` nao estao instalados nesta maquina; os benchmarks abaixo usam `std::time::Instant` com repeticoes e mediana (o protocolo, nao a ferramenta, e o exercicio), e os itens 4 e 7 ficam como roteiro documentado — nao executados aqui.

```rust
use std::sync::atomic::{AtomicU64, Ordering};
use std::time::{Duration, Instant};

fn mediana(mut tempos: Vec<Duration>) -> Duration {
    tempos.sort();
    tempos[tempos.len() / 2]
}

fn mede<T, F: FnMut() -> T>(nome: &str, mut f: F) {
    let mut tempos = Vec::new();
    for _ in 0..5 {
        let inicio = Instant::now();
        let resultado = f();
        tempos.push(inicio.elapsed());
        std::hint::black_box(resultado);
    }
    println!("{}: mediana {:?}", nome, mediana(tempos));
}

#[derive(Clone, Copy)]
struct Ponto {
    a: f64,
    b: f64,
    c: f64,
    d: f64,
    e: f64,
    f: f64,
}

#[derive(Debug)]
struct Padrao {
    a: u8,
    b: u32,
    c: u64,
    d: f64,
    e: bool,
    f: u16,
    g: u8,
}

#[derive(Debug)]
#[repr(C)]
struct Original {
    a: u8,
    b: u32,
    c: u64,
    d: f64,
    e: bool,
    f: u16,
    g: u8,
}

#[derive(Debug)]
struct Otimizada {
    c: u64,
    d: f64,
    b: u32,
    f: u16,
    a: u8,
    e: bool,
    g: u8,
}

fn soma_f32_chunks(v: &[f32]) -> f32 {
    let mut acc = [0.0f32; 8];
    let blocos = v.chunks_exact(8);
    let resto = blocos.remainder();
    for bloco in blocos {
        for i in 0..8 {
            acc[i] += bloco[i];
        }
    }
    let mut total: f32 = acc.iter().sum();
    for x in resto {
        total += x;
    }
    total
}

fn main() {
    // Exercicio 1 (sem criterion): sorts na mesma entrada
    let base: Vec<i32> = (0..100_000).rev().collect();
    mede("sort", || {
        let mut v = base.clone();
        v.sort();
        v
    });
    mede("sort_unstable", || {
        let mut v = base.clone();
        v.sort_unstable();
        v
    });

    // Exercicio 2: AoS contra SoA somando um campo
    let pontos: Vec<Ponto> = (0..1_000_000)
        .map(|i| Ponto { a: i as f64, b: 0.0, c: 0.0, d: 0.0, e: 0.0, f: 0.0 })
        .collect();
    let campo_a: Vec<f64> = pontos.iter().map(|p| p.a).collect();
    mede("AoS soma campo", || pontos.iter().map(|p| p.a).sum::<f64>());
    mede("SoA soma campo", || campo_a.iter().sum::<f64>());
    let checksum: f64 = pontos.iter().map(|p| p.b + p.c + p.d + p.e + p.f).sum();
    println!("checksum dos demais campos: {}", checksum);

    // Exercicio 3: false sharing, contadores colados contra alinhados
    fn incrementa(parte: &(AtomicU64, AtomicU64), qual: usize) {
        for _ in 0..200_000 {
            if qual == 0 {
                parte.0.fetch_add(1, Ordering::Relaxed);
            } else {
                parte.1.fetch_add(1, Ordering::Relaxed);
            }
        }
    }
    #[repr(align(64))]
    struct Alinhado(AtomicU64);

    let colados = (AtomicU64::new(0), AtomicU64::new(0));
    mede("colados (2 threads)", || {
        std::thread::scope(|s| {
            s.spawn(|| incrementa(&colados, 0));
            s.spawn(|| incrementa(&colados, 1));
        });
    });
    let separados = (Alinhado(AtomicU64::new(0)), Alinhado(AtomicU64::new(0)));
    mede("separados (2 threads)", || {
        std::thread::scope(|s| {
            s.spawn(|| {
                for _ in 0..200_000 {
                    separados.0.0.fetch_add(1, Ordering::Relaxed);
                }
            });
            s.spawn(|| {
                for _ in 0..200_000 {
                    separados.1.0.fetch_add(1, Ordering::Relaxed);
                }
            });
        });
    });

    // Exercicio 5: soma simples contra chunks de 8
    let valores: Vec<f32> = (0..1_000_003).map(|i| (i % 7) as f32).collect();
    mede("soma simples", || valores.iter().sum::<f32>());
    mede("soma chunks8", || soma_f32_chunks(&valores));
    let simples = valores.iter().sum::<f32>();
    println!("somas conferem (aprox): {}", (simples - soma_f32_chunks(&valores)).abs() < 1.0);

    // Exercicio 6: tamanhos de layout (instancias para o compilador nao
    // tratar os campos como mortos; o assunto aqui e o tamanho)
    let instancias = (
        Padrao { a: 1, b: 2, c: 3, d: 4.0, e: true, f: 5, g: 6 },
        Original { a: 1, b: 2, c: 3, d: 4.0, e: true, f: 5, g: 6 },
        Otimizada { c: 3, d: 4.0, b: 2, f: 5, a: 1, e: true, g: 6 },
    );
    fn toque(soma: u64, campos: [u64; 7]) -> u64 {
        soma + campos.iter().sum::<u64>()
    }
    let tocado = toque(0, [
        instancias.0.a as u64, instancias.0.b as u64, instancias.0.c,
        instancias.0.d as u64, instancias.0.e as u64, instancias.0.f as u64,
        instancias.0.g as u64,
    ]) + toque(0, [
        instancias.1.a as u64, instancias.1.b as u64, instancias.1.c,
        instancias.1.d as u64, instancias.1.e as u64, instancias.1.f as u64,
        instancias.1.g as u64,
    ]) + toque(0, [
        instancias.2.a as u64, instancias.2.b as u64, instancias.2.c,
        instancias.2.d as u64, instancias.2.e as u64, instancias.2.f as u64,
        instancias.2.g as u64,
    ]);
    println!("campos tocados (soma simbolica): {}", tocado);
    println!("tamanhos: padrao={} reprC={} otimizada={}",
             std::mem::size_of::<Padrao>(),
             std::mem::size_of::<Original>(),
             std::mem::size_of::<Otimizada>());
    assert!(std::mem::size_of::<Otimizada>() <= std::mem::size_of::<Original>());
    println!("medicoes desta maquina; criterion/flamegraph/PGO: ver texto");
}
```

Leituras que voce deve fazer dos seus numeros (nesta maquina, rode e anote os seus): `sort_unstable` tende a vencer `sort` onde nao ha estabilidade a preservar; SoA vence AoS ao somar um campo porque cada linha de cache so traz o campo; contadores colados pagam coerencia a cada incremento alheio. Se os seus numeros discordarem da tendencia, o experimento continua valido — a explicacao precisa mudar, nao o dado.

4 e 7 (roteiros nao executados neste ambiente): com `cargo flamegraph`, gere o grafico do programa com gargalo escondido, corrija a funcao dominante e meça de novo com o mesmo protocolo. Com PGO: compile com `-Cprofile-generate`, rode a carga representativa, recompile com `-Cprofile-use` e compare medianas — documente a carga de treino junto do percentual, porque PGO treinado na carga errada mede o passado errado.

6. A struct otimizada agrupa campos por alinhamento decrescente e encolhe o preenchimento: confira com `size_of` antes e depois na sua maquina e some os buracos a mao uma vez — o numero deixa de ser magia.

**Autoverificacao:** saidas ordenadas/somas conferidas antes dos tempos, mediana de 5 repeticoes e cada grafico/percentual acompanhado da carga que o produziu.
