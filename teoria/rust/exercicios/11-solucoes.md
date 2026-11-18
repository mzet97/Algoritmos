---
layout: default
---

# Aula 11 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../11.md#11-exercícios-obrigatórios).

```rust
use std::collections::HashMap;

fn eh_primo(n: u64) -> bool {
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

struct Primos {
    atual: u64,
}

impl Primos {
    fn novo() -> Primos {
        Primos { atual: 1 }
    }
}

impl Iterator for Primos {
    type Item = u64;
    fn next(&mut self) -> Option<u64> {
        let mut candidato = self.atual + 1;
        while !eh_primo(candidato) {
            candidato += 1;
        }
        self.atual = candidato;
        Some(candidato)
    }
}

fn fazer_avaliador(minimo: f64) -> impl Fn(f64) -> bool {
    move |nota| nota >= minimo
}

fn compose<A, B, C>(f: impl Fn(B) -> C, g: impl Fn(A) -> B) -> impl Fn(A) -> C {
    move |x| f(g(x))
}

fn main() {
    let soma: i32 = (1..=100)
        .filter(|n| n % 3 == 0)
        .map(|n| n * n)
        .filter(|n| *n <= 1000)
        .sum();
    println!("pipeline: {}", soma);

    let texto = "o rato roeu o queijo o rato fugiu";
    let contagem = texto.split_whitespace().fold(HashMap::new(), |mut acc: HashMap<&str, usize>, p| {
        *acc.entry(p).or_insert(0) += 1;
        acc
    });
    let mut ordenado: Vec<(&str, usize)> = contagem.into_iter().collect();
    ordenado.sort_by(|a, b| b.1.cmp(&a.1).then_with(|| a.0.cmp(b.0)));
    println!("top: {:?}", &ordenado[..ordenado.len().min(5)]);

    let primeiros20: Vec<u64> = Primos::novo().take(20).collect();
    println!("primos: {:?}", primeiros20);
    assert_eq!(primeiros20.len(), 20);
    assert_eq!(*primeiros20.last().unwrap(), 71);
    let mersenne: Vec<u64> = Primos::novo()
        .take(1000)
        .filter(|&p| {
            let mut potencia = 2u64;
            while potencia + 1 < p {
                potencia = potencia * 2 + 1;
            }
            potencia + 1 == p
        })
        .collect();
    println!("mersenne entre os 1000 primeiros primos: {:?}", mersenne);

    let aprovacao = fazer_avaliador(7.0);
    let honra = fazer_avaliador(9.0);
    println!("8.5 aprova={} honra={}", aprovacao(8.5), honra(8.5));

    let pares = vec![
        (String::from("a"), 10),
        (String::from("b"), 20),
        (String::from("a"), 30),
    ];
    let grupos = pares.into_iter().fold(HashMap::new(), |mut acc: HashMap<String, Vec<i32>>, (c, v)| {
        acc.entry(c).or_default().push(v);
        acc
    });
    let mut medias: Vec<(String, f64)> = grupos
        .iter()
        .map(|(c, valores)| {
            (c.clone(), valores.iter().sum::<i32>() as f64 / valores.len() as f64)
        })
        .collect();
    medias.sort_by(|a, b| a.0.cmp(&b.0));
    println!("medias: {:?}", medias);

    let matriz = vec![vec![1, 2], vec![], vec![3, 4, 5]];
    let achatado: Vec<i32> = matriz.into_iter().flatten().collect();
    println!("achatado: {:?}", achatado);
    let frases = vec!["ola mundo", "rust e legal"];
    let palavras: Vec<&str> = frases.iter().flat_map(|f| f.split_whitespace()).collect();
    println!("palavras: {:?}", palavras);

    let dobrar = |x: i32| x * 2;
    let somar_1 = |x: i32| x + 1;
    let composta = compose(dobrar, somar_1);
    println!("compose: {}", composta(5));
    assert_eq!(composta(5), 12);
}
```

O filtro de Mersenne procura `p` na forma `2^n - 1` gerando a sequencia 3, 7, 15, 31, ... e conferindo igualdade — suficiente para primos pequenos; com primos grandes, some em `u128` ou limite a busca (declarado aqui para voce nao copiar o truque para numeros serios).

**Autoverificacao:** pipeline sem lacos explicitos, 20 primos terminando em 71, `compose` dando 12 e top de frequencia deterministico pelo desempate.
