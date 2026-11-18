---
layout: default
---

# Aula 09 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../09.md#11-exercícios-obrigatórios).

```rust
use std::fmt;
use std::ops::{Add, Div, Mul, Sub};

fn min_max<T: Ord>(slice: &[T]) -> Option<(&T, &T)> {
    let mut iter = slice.iter();
    let primeiro = iter.next()?;
    let mut min = primeiro;
    let mut max = primeiro;
    for item in iter {
        if item < min {
            min = item;
        }
        if item > max {
            max = item;
        }
    }
    Some((min, max))
}

trait Serializar {
    fn serializar(&self) -> String;
}

impl Serializar for i32 {
    fn serializar(&self) -> String {
        format!("i32:{}", self)
    }
}
impl Serializar for f64 {
    fn serializar(&self) -> String {
        format!("f64:{}", self)
    }
}
impl Serializar for String {
    fn serializar(&self) -> String {
        format!("str:{}", self)
    }
}
impl Serializar for bool {
    fn serializar(&self) -> String {
        format!("bool:{}", if *self { 1 } else { 0 })
    }
}

fn serializar_tudo<T: Serializar>(itens: &[T]) -> Vec<String> {
    itens.iter().map(|i| i.serializar()).collect()
}

struct Pilha<T> {
    itens: Vec<T>,
}

impl<T> Pilha<T> {
    fn nova() -> Pilha<T> {
        Pilha { itens: Vec::new() }
    }
    fn push(&mut self, item: T) {
        self.itens.push(item);
    }
    fn pop(&mut self) -> Option<T> {
        self.itens.pop()
    }
    fn peek(&self) -> Option<&T> {
        self.itens.last()
    }
    fn is_empty(&self) -> bool {
        self.itens.is_empty()
    }
    fn len(&self) -> usize {
        self.itens.len()
    }
    fn clear(&mut self) {
        self.itens.clear();
    }
}

fn mdc(a: i64, b: i64) -> i64 {
    let (mut a, mut b) = (a.abs(), b.abs());
    while b != 0 {
        (a, b) = (b, a % b);
    }
    a.max(1)
}

#[derive(Debug, Clone, Copy)]
struct Fracao {
    num: i64,
    den: i64,
}

impl Fracao {
    fn nova(num: i64, den: i64) -> Fracao {
        assert!(den != 0, "denominador zero");
        let sinal = if den < 0 { -1 } else { 1 };
        let d = mdc(num, den);
        Fracao { num: sinal * num / d, den: (den * sinal) / d }
    }
}

impl From<(i64, i64)> for Fracao {
    fn from(par: (i64, i64)) -> Fracao {
        Fracao::nova(par.0, par.1)
    }
}

impl Add for Fracao {
    type Output = Fracao;
    fn add(self, outro: Fracao) -> Fracao {
        Fracao::nova(self.num * outro.den + outro.num * self.den, self.den * outro.den)
    }
}
impl Sub for Fracao {
    type Output = Fracao;
    fn sub(self, outro: Fracao) -> Fracao {
        Fracao::nova(self.num * outro.den - outro.num * self.den, self.den * outro.den)
    }
}
impl Mul for Fracao {
    type Output = Fracao;
    fn mul(self, outro: Fracao) -> Fracao {
        Fracao::nova(self.num * outro.num, self.den * outro.den)
    }
}
impl Div for Fracao {
    type Output = Fracao;
    fn div(self, outro: Fracao) -> Fracao {
        Fracao::nova(self.num * outro.den, self.den * outro.num)
    }
}
impl PartialEq for Fracao {
    fn eq(&self, outro: &Fracao) -> bool {
        self.num == outro.num && self.den == outro.den
    }
}
impl fmt::Display for Fracao {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}/{}", self.num, self.den)
    }
}

trait Desenhavel {
    fn desenhar(&self) -> String;
}

struct Circulo;
struct Quadrado;
struct Texto(String);

impl Desenhavel for Circulo {
    fn desenhar(&self) -> String {
        String::from("circulo")
    }
}
impl Desenhavel for Quadrado {
    fn desenhar(&self) -> String {
        String::from("quadrado")
    }
}
impl Desenhavel for Texto {
    fn desenhar(&self) -> String {
        format!("texto:{}", self.0)
    }
}

trait Grafico {
    type Dado;
    fn plotar(&self, dados: &[Self::Dado]) -> String;
}

struct GraficoBarras;
struct GraficoLinha;

impl Grafico for GraficoBarras {
    type Dado = f64;
    fn plotar(&self, dados: &[f64]) -> String {
        format!("barras[{}]", dados.len())
    }
}
impl Grafico for GraficoLinha {
    type Dado = (f64, f64);
    fn plotar(&self, dados: &[(f64, f64)]) -> String {
        format!("linha[{}]", dados.len())
    }
}

fn map_option<T, U, F>(opt: Option<T>, f: F) -> Option<U>
where
    F: FnOnce(T) -> U,
{
    match opt {
        Some(v) => Some(f(v)),
        None => None,
    }
}

fn filter_result<T, E, F>(res: Result<T, E>, pred: F) -> Result<T, E>
where
    F: FnOnce(&T) -> bool,
{
    match res {
        Ok(v) if pred(&v) => Ok(v),
        Ok(_) => panic!("filter_result sem erro para rejeicao: ver comentario"),
        Err(e) => Err(e),
    }
}

fn partition_result<T, E>(items: Vec<Result<T, E>>) -> (Vec<T>, Vec<E>) {
    let mut oks = Vec::new();
    let mut erros = Vec::new();
    for item in items {
        match item {
            Ok(v) => oks.push(v),
            Err(e) => erros.push(e),
        }
    }
    (oks, erros)
}

fn main() {
    println!("min_max: {:?}", min_max(&[3, 1, 4, 1, 5]));
    println!("serializados: {:?}", serializar_tudo(&[1i32, -2, 30]));

    let mut pilha = Pilha::nova();
    pilha.push(10);
    pilha.push(20);
    let topo = pilha.peek().copied();
    let removido = pilha.pop();
    println!("pilha: len={} peek={:?} pop={:?} vazia={}",
             pilha.len(), topo, removido, pilha.is_empty());
    pilha.clear();
    println!("pilha apos clear: len={}", pilha.len());

    let soma = Fracao::from((1, 2)) + Fracao::from((1, 3));
    println!("fracao: {} == 5/6? {}", soma, soma == Fracao::from((5, 6)));
    assert_eq!(soma, Fracao::from((5, 6)));
    assert_eq!(Fracao::from((2, 4)), Fracao::from((1, 2)));

    let formas: Vec<Box<dyn Desenhavel>> = vec![
        Box::new(Circulo),
        Box::new(Quadrado),
        Box::new(Texto(String::from("ola"))),
    ];
    for forma in &formas {
        println!("desenha: {}", forma.desenhar());
    }

    println!("graficos: {} {}", GraficoBarras.plotar(&[1.0, 2.0]), GraficoLinha.plotar(&[(1.0, 2.0)]));

    println!("map_option: {:?}", map_option(Some(3), |x| x * 2));
    let (oks, erros) = partition_result(vec![Ok(1), Err("e1"), Ok(2), Err("e2")]);
    println!("partition: {:?} {:?}", oks, erros);
    assert_eq!(oks, vec![1, 2]);
    assert_eq!(erros, vec!["e1", "e2"]);
    assert_eq!(filter_result(Ok::<i32, String>(10), |v| *v > 5), Ok(10));
}
```

Uma decisao que o exercicio 7 nao fecha: `filter_result` nao tem como devolver "rejeitado" sem um erro — a assinatura `Result<T, E>` nao carrega um terceiro estado. Esta solucao trata rejeicao como erro de programacao (panic documentado); desenhos reais devolvem `Result<Option<T>, E>` ou recebem um erro de rejeicao. Assinatura tambem e contrato, e este e o ponto do exercicio.

**Autoverificacao:** fracao simplificada comparando igual a `5/6`, pilha com `clear` zerando e a particionacao separando 2 oks de 2 erros.
