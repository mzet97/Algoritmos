# Aula 23a — Baseline correto, multicore honesto e a conta da GPU

---

## Objetivo da Aula

Medir um núcleo numérico em Rust com método — hipótese, baseline verificado, aquecimento, mínimo de repetições, variância, ambiente anotado — e decidir no papel se um acelerador valeria a transferência. Pré-requisitos: aulas 12a a 16. Não precisa de `wgpu`, CUDA, `rust-gpu`, nightly nem placa.

Ao terminar, você deverá comparar laço sequencial, `std::thread::scope` e um atômico compartilhado com a mesma invariante verificada, explicar a curva com Amdahl e banda, e emitir o veredito da GPU a partir da aritmética, não do marketing.

Esta aula é o núcleo executável do módulo R7: referência correta, variante multicore, relatório de variância e modelo de custo com `&[T]` e `f64::EPSILON` onde couber — tudo em CPU estável, sem `nightly`. `wgpu`/CUDA/`rust-gpu` ficam nos laboratórios opcionais.

## 1. O protocolo, antes do cronômetro

Hipótese escrita, baseline verificado contra soma fechada, aquecimento fora da conta, mínimo de N rodadas (o mínimo filtra ruído para baixo), consumo opaco do resultado com `std::hint::black_box` — sem ele, o otimizador prova que as repetições repetem e o cronômetro mede zero — e ambiente anotado: toolchain (`rustc --version`), edição, perfil (`debug` x `release` — este exemplo exige `--release` para números honestos), SO, CPUs e carga.

## 2. Onde o paralelismo mora em Rust seguro

`std::thread::scope` permite emprestar `&[u64]` às threads filhas com garantia de `join` — sem `Arc`, sem `unsafe`, sem lifetime manual. Partições independentes somam em paralelo de verdade; um `AtomicU64` compartilhado soma certo e devagar (contenção na linha de cache). O borrow checker aprova os dois; só o cronômetro distingue.

## 3. Amdahl e o teto antes da medição

Fração paralelizável `p`, N threads: teto `1/((1-p)+p/N)`, antes de contenção, comunicação e desbalanceamento. Para redução particionada, `p ≈ 1` e o teto prático é a banda de memória: N núcleos famintos dividem o mesmo barramento.

## 4. O modelo de custo do acelerador

`t_total = t_ida + t_kernel + t_volta`, com banda declarada. A regra honesta: só atravessa o barramento quem já provou que o cálculo local é o gargalo — e a prova é um número com ambiente anexado, não um pressentimento.

## 5. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/23a_hpc.rs) contém exatamente o programa abaixo. Compile em modo `release` (números de `debug` medem o otimizador desligado, não o programa):

```bash
rustc --edition 2024 -O -W warnings teoria/rust/exemplos/23a_hpc.rs -o hpc23a
./hpc23a
```

```rust
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
```

Saída esperada (tempos variam por máquina, carga, CPUs e perfil):

```text
seq: ~s
par2: ~s speedup=~x
par4: ~s speedup=~x
par8: ~s speedup=~x
contendido4: ~s (granularidade grossa: 4 adds atomicos no total)
viagem=0.0186s kernel=0.001s total=0.0196s
veredito: ...
hpc ok: baseline verificado, escala medida, conta feita
```

Leia com o modelo certo: a soma particionada tende a escalar até a banda saturar; a versão "contendida" aqui faz *uma* adição atômica por fatia — granularidade grossa de propósito, para mostrar que átomo não é o vilão, e sim a frequência. (Com um `fetch_add` por elemento, o resultado seria ordens de magnitude pior: experimente no exercício 2.) O veredito da GPU sai da aritmética e muda com o ambiente medido — nunca foi propriedade do código.

### O que a memória faz aqui?

O vetor mora no heap, dono único no `main`; as threads filhas tomam `&[u64]` emprestado com tempo de vida provado pelo `scope` — sem `Arc`, sem `clone`, sem contagem de referências. `parcial` é escrito uma vez por thread em posições distintas e só lido após o `join` implícito do fim do `scope`: nenhuma corrida possível, e o compilador sabe disso.

## 6. Erros comuns e implicações defensivas

1. **Medir em `debug`:** sem otimização o número mede o otimizador desligado. Benchmark sempre em `release`.
2. **Medir uma vez:** turbo, GC do SO e vizinhos dançam. Mínimo de N rodadas + ambiente.
3. **Medir sem invariante:** cada variante afirma igualdade com a baseline. Sem isso, o benchmark pode cronometrar o erro.
4. **Confundir átomo com contenção:** `fetch_add` por elemento serializa; por fatia, some 4 no total. Granularidade, não dogma.
5. **Alegar GPU sem a viagem:** some ida e volta com banda declarada. Sem isso, fantasia.
6. **Prometer escala linear:** Amdahl + banda dizem não antes de você medir. Preveja, confira.

## 7. Ponte com as demais trilhas

C mede o mesmo com pthreads (aula 28a); Go com worker pool e race detector (aula 28a da trilha Go); Python com NumPy contra laço (aula 29a). O método — hipótese, baseline, mínimo, ambiente — e a conclusão sobre a viagem são os mesmos em toda parte.

## 8. Exercícios obrigatórios

1. Varie fatias (1, 2, 4, 8, 16) com o mesmo total e explique a curva com Amdahl e banda.
2. Mude a versão contendora para um `fetch_add` por elemento e meça o estrago. Explique com linhas de cache.
3. Rode em `debug` e compare com `release`. Explique a diferença antes de chamar qualquer número de resultado.
4. Refaça a conta da viagem com banda de 100 GiB/s e kernel de 10 ms. O veredito muda? Mostre a conta.

Use [pistas graduais](exercicios/23a_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/23a_solucoes.md) depois de registrar sua própria explicação.

## 9. Critério de aprovação e checklist

Você concluiu a aula se consegue, sem olhar a solução:

- enunciar hipótese, baseline, aquecimento, repetição e ambiente antes de cronometrar;
- explicar por que `black_box` é necessário e o que ele impede o compilador de fazer;
- distinguir contenção real de granularidade grossa com números;
- fazer a conta da viagem e emitir o veredito com os números;
- compilar sem warnings, passar no `clippy` e confrontar cada número com o modelo.

**Próxima aula:** backend de alta performance. O raciocínio desta aula — medir antes de escalar — decide o que paralelizar no serviço.
