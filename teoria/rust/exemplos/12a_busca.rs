/// Devolve o indice de `alvo` em `valores` ou `None` se ausente.
///
/// Pre-condicao: `valores` ordenada em ordem nao decrescente.
/// Invariante: se `alvo` esta na fatia, esta em `valores[ini..fim]`.
fn busca_binaria(valores: &[i32], alvo: i32) -> Option<usize> {
    let mut ini = 0usize;
    let mut fim = valores.len();
    while ini < fim {
        let meio = ini + (fim - ini) / 2;
        if valores[meio] == alvo {
            return Some(meio);
        }
        if alvo < valores[meio] {
            fim = meio;
        } else {
            ini = meio + 1;
        }
    }
    None
}

fn busca_linear(valores: &[i32], alvo: i32) -> Option<usize> {
    valores.iter().position(|&v| v == alvo)
}

/// Gerador deterministico simples (LCG): sem crates externas, semente fixa.
struct Lcg(u64);

impl Lcg {
    fn proximo(&mut self, limite: i32) -> i32 {
        self.0 = self.0.wrapping_mul(6364136223846793005).wrapping_add(1442695040888963407);
        ((self.0 >> 33) as i32).rem_euclid(limite)
    }
}

fn checar_propriedade(tentativas: u32) {
    let mut rng = Lcg(42); // semente fixa: a falha e reproduzivel
    for _ in 0..tentativas {
        let tamanho = rng.proximo(51) as usize;
        let mut valores: Vec<i32> = (0..tamanho as u32).map(|_| rng.proximo(201) - 100).collect();
        valores.sort();
        let alvo = rng.proximo(221) - 110;
        match busca_binaria(&valores, alvo) {
            Some(i) => assert_eq!(valores[i], alvo, "indice invalido"),
            None => assert!(!valores.contains(&alvo), "falso negativo"),
        }
    }
}

/// Oraculo: a busca da biblioteca padrao, exercitada sobre os mesmos casos.
fn checar_contra_std() {
    let mut rng = Lcg(7);
    for _ in 0..500 {
        let tamanho = rng.proximo(51) as usize;
        let mut valores: Vec<i32> = (0..tamanho as u32).map(|_| rng.proximo(201) - 100).collect();
        valores.sort();
        let alvo = rng.proximo(221) - 110;
        assert_eq!(
            busca_binaria(&valores, alvo).is_some(),
            valores.binary_search(&alvo).is_ok()
        );
    }
}

fn main() {
    assert_eq!(busca_binaria(&[], 5), None);
    assert_eq!(busca_binaria(&[5], 5), Some(0));
    assert_eq!(busca_binaria(&[5], 4), None);
    assert_eq!(busca_binaria(&[1, 3, 5, 7], 1), Some(0));
    assert_eq!(busca_binaria(&[1, 3, 5, 7], 7), Some(3));
    assert_eq!(busca_binaria(&[1, 3, 5, 7], 4), None);
    assert!(busca_binaria(&[2, 2, 2], 2).is_some_and(|i| [0, 1, 2].contains(&i)));

    checar_propriedade(2000);
    checar_contra_std();

    let grande: Vec<i32> = (0..1_000_000).map(|x| x * 2).collect();
    for f in [busca_linear, busca_binaria] {
        assert_eq!(f(&grande, 1_999_998), Some(999_999));
        assert_eq!(f(&grande, 1_999_999), None);
    }

    let rep_lin = 5;
    let inicio = std::time::Instant::now();
    for _ in 0..rep_lin {
        busca_linear(&grande, 1_999_999);
    }
    let t_lin = inicio.elapsed().as_secs_f64() / rep_lin as f64;

    let rep_bin = 200;
    let inicio = std::time::Instant::now();
    for _ in 0..rep_bin {
        busca_binaria(&grande, 1_999_999);
    }
    let t_bin = inicio.elapsed().as_secs_f64() / rep_bin as f64;

    println!("linear: {t_lin:.6}s/media ({rep_lin}x)");
    println!("binaria: {t_bin:.9}s/media ({rep_bin}x)");
}
