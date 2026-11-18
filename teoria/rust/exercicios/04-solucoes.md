---
layout: default
---

# Aula 04 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../04.md#9-exercícios-obrigatórios).

```rust
fn estatisticas(slice: &[f64]) -> (f64, f64, f64, f64) {
    let n = slice.len() as f64;
    let min = slice.iter().copied().fold(f64::INFINITY, f64::min);
    let max = slice.iter().copied().fold(f64::NEG_INFINITY, f64::max);
    let media = slice.iter().sum::<f64>() / n;
    let variancia = slice.iter().map(|x| (x - media) * (x - media)).sum::<f64>() / n;
    (min, max, media, variancia.sqrt())
}

fn busca_binaria(slice: &[i32], alvo: i32) -> Option<usize> {
    let (mut lo, mut hi) = (0usize, slice.len());
    while lo < hi {
        let meio = lo + (hi - lo) / 2;
        if slice[meio] < alvo {
            lo = meio + 1;
        } else if slice[meio] > alvo {
            hi = meio;
        } else {
            return Some(meio);
        }
    }
    None
}

fn contar_palavras(texto: &str) -> usize {
    texto.split_whitespace().count()
}

fn palavra_mais_longa<'a>(texto: &'a str) -> &'a str {
    texto.split_whitespace().max_by_key(|p| p.chars().count()).unwrap_or("")
}

fn inverter(slice: &mut [i32]) {
    let n = slice.len();
    for i in 0..n / 2 {
        slice.swap(i, n - 1 - i);
    }
}

fn analisar_utf8(s: &str) -> (usize, usize, usize) {
    (s.len(), s.chars().count(), s.chars().filter(|c| c.is_ascii()).count())
}

fn merge(a: &[i32], b: &[i32]) -> Vec<i32> {
    let mut saida = Vec::with_capacity(a.len() + b.len());
    let (mut i, mut j) = (0, 0);
    while i < a.len() && j < b.len() {
        if a[i] <= b[j] {
            saida.push(a[i]);
            i += 1;
        } else {
            saida.push(b[j]);
            j += 1;
        }
    }
    saida.extend_from_slice(&a[i..]);
    saida.extend_from_slice(&b[j..]);
    saida
}

fn construir_csv(campos: &[&str]) -> String {
    let partes: Vec<String> = campos
        .iter()
        .map(|c| {
            if c.contains(',') {
                format!("\"{}\"", c)
            } else {
                c.to_string()
            }
        })
        .collect();
    partes.join(",")
}

fn main() {
    let (min, max, media, desvio) = estatisticas(&[2.0, 4.0, 6.0]);
    println!("min={} max={} media={} desvio={:.4}", min, max, media, desvio);

    assert_eq!(busca_binaria(&[1, 3, 5, 7, 9], 7), Some(3));
    assert_eq!(busca_binaria(&[1, 3, 5, 7, 9], 4), None);
    assert_eq!(contar_palavras("um dois tres"), 3);
    assert_eq!(palavra_mais_longa("gato cachorro peixe"), "cachorro");
    let mut v = [1, 2, 3, 4];
    inverter(&mut v);
    assert_eq!(v, [4, 3, 2, 1]);
    println!("utf8 'ola mundo': {:?}", analisar_utf8("ola mundo"));
    println!("utf8 com coracao: {:?}", analisar_utf8("coração 💻"));
    assert_eq!(merge(&[1, 3, 5], &[2, 4, 6]), vec![1, 2, 3, 4, 5, 6]);
    assert_eq!(construir_csv(&["nome", "idade"]), "nome,idade");
    assert_eq!(construir_csv(&["Silva, Jr", "30"]), "\"Silva, Jr\",30");
    println!("testes ok");
}
```

Decisoes que o enunciado nao fecha e esta solucao declara: `estatisticas` de slice vazio devolve infinito/NaN pelas formulas (quem chama garante nao-vazio); `palavra_mais_longa` usa `max_by_key`, que em empate devolve a **ultima** palavra maxima (comportamento padrao documentado da std); no CSV, aspas dentro do campo nao sao escapadas, entao o formato deste laboratorio so aceita campos sem aspas — CSV completo (RFC 4180) dobraria as aspas internas.

5. Em `"coração 💻"`: bytes > chars > ASCII — cada `ç`/`ã` ocupa 2 bytes e o emoji, 4. E por isso que indice de byte nao e indice de caractere, como a [aula 03](03-solucoes.md) corrige.

**Autoverificacao:** asserts verdes, merge sem `.sort()`, e as tres contagens UTF-8 diferentes no segundo exemplo.
