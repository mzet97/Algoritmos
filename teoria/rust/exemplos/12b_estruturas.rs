// Aula 12b — arvores e ordenacao em Rust: Box por no, oraculo da std.
use std::cmp::Ordering;

struct Estado(u64);
impl Estado {
    fn prox(&mut self) -> u32 {
        self.0 = self.0.wrapping_mul(6364136223846793005).wrapping_add(1);
        (self.0 >> 33) as u32
    }
}

type Link = Option<Box<No>>;
struct No {
    chave: i32,
    esq: Link,
    dir: Link,
}

fn inserir(link: &mut Link, chave: i32) -> bool {
    match link {
        None => {
            *link = Some(Box::new(No { chave, esq: None, dir: None }));
            true
        }
        Some(no) => match chave.cmp(&no.chave) {
            Ordering::Equal => false,
            Ordering::Less => inserir(&mut no.esq, chave),
            Ordering::Greater => inserir(&mut no.dir, chave),
        },
    }
}

fn contem(link: &Link, chave: i32) -> bool {
    match link {
        None => false,
        Some(no) => match chave.cmp(&no.chave) {
            Ordering::Equal => true,
            Ordering::Less => contem(&no.esq, chave),
            Ordering::Greater => contem(&no.dir, chave),
        },
    }
}

fn em_ordem(link: &Link, saida: &mut Vec<i32>) {
    if let Some(no) = link {
        em_ordem(&no.esq, saida);
        saida.push(no.chave);
        em_ordem(&no.dir, saida);
    }
}

fn altura(link: &Link) -> usize {
    match link {
        None => 0,
        Some(no) => 1 + altura(&no.esq).max(altura(&no.dir)),
    }
}

fn main() {
    // Propriedade: em-ordem da BST == sort da std (com duplicatas fora).
    let mut est = Estado(42);
    for _ in 0..300 {
        let n = (est.prox() % 60) as usize;
        let mut valores: Vec<i32> = (0..n).map(|_| (est.prox() % 40) as i32 - 20).collect();
        let mut arv: Link = None;
        let mut distintas = 0;
        for &v in &valores {
            if inserir(&mut arv, v) {
                distintas += 1;
            }
            assert!(contem(&arv, v));
        }
        valores.sort_unstable();
        valores.dedup();
        let mut saida = Vec::new();
        em_ordem(&arv, &mut saida);
        assert_eq!(saida, valores);
        assert_eq!(saida.len(), distintas);
    }
    println!("propriedade ok: BST em-ordem == sort_unstable+dedup (300 casos)");

    // Degeneracao medida: ordenado vira lista; embaralhado fica baixo.
    let mut lista: Link = None;
    for i in 0..1000 {
        assert!(inserir(&mut lista, i));
    }
    assert_eq!(altura(&lista), 1000);
    let mut perm: Vec<i32> = (0..1000).collect();
    for i in (1..1000).rev() {
        let j = (est.prox() as usize) % (i + 1);
        perm.swap(i, j);
    }
    let mut emb: Link = None;
    for &v in &perm {
        assert!(inserir(&mut emb, v));
    }
    let h = altura(&emb);
    println!("altura ordenada=1000 embaralhada={h}");
    assert!(h < 100);

    // Ordenacao: a std e o oraculo; medir o custo de Box por no vs vetor.
    let mut v: Vec<i32> = (0..100_000).map(|_| (est.prox() % 10000) as i32).collect();
    let mut esperado = v.clone();
    let t0 = std::time::Instant::now();
    v.sort_unstable();
    let t_vetor = t0.elapsed();
    esperado.sort_unstable();
    assert_eq!(v, esperado);
    println!("sort_unstable 100k em {t_vetor:?} (vetor contiguo, sem Box por elemento)");
    println!("12b ok: BST com Box, degeneracao medida e oraculo da std");
}
