---
layout: default
---

# Aula 05 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../05.md#9-exercícios-obrigatórios).

```rust
#[derive(Debug, PartialEq)]
struct Retangulo {
    largura: f64,
    altura: f64,
}

impl Retangulo {
    fn area(&self) -> f64 {
        self.largura * self.altura
    }
    fn perimetro(&self) -> f64 {
        2.0 * (self.largura + self.altura)
    }
    fn pode_conter(&self, outro: &Retangulo) -> bool {
        self.largura >= outro.largura && self.altura >= outro.altura
    }
    fn quadrado(lado: f64) -> Retangulo {
        Retangulo { largura: lado, altura: lado }
    }
}

#[derive(Debug)]
enum Forma2D {
    Circulo(f64),
    Retangulo { largura: f64, altura: f64 },
    Triangulo { base: f64, altura: f64 },
}

fn area(f: &Forma2D) -> f64 {
    match f {
        Forma2D::Circulo(r) => std::f64::consts::PI * r * r,
        Forma2D::Retangulo { largura, altura } => largura * altura,
        Forma2D::Triangulo { base, altura } => base * altura / 2.0,
    }
}

fn dividir_se_positivo(a: i32, b: i32) -> Option<i32> {
    if a > 0 && b > 0 {
        Some(a / b)
    } else {
        None
    }
}

#[derive(Debug)]
enum Lista {
    Cons(i32, Box<Lista>),
    Nil,
}

fn comprimento(lista: &Lista) -> i32 {
    match lista {
        Lista::Cons(_, resto) => 1 + comprimento(resto),
        Lista::Nil => 0,
    }
}

fn soma(lista: &Lista) -> i32 {
    match lista {
        Lista::Cons(x, resto) => x + soma(resto),
        Lista::Nil => 0,
    }
}

fn ultimo(lista: &Lista) -> Option<i32> {
    match lista {
        Lista::Cons(x, resto) => match **resto {
            Lista::Nil => Some(*x),
            _ => ultimo(resto),
        },
        Lista::Nil => None,
    }
}

#[derive(Debug, PartialEq)]
enum Token {
    Numero(f64),
    Operador(char),
    Parenteses(char),
    Identificador(String),
}

fn tokenizar(input: &str) -> Vec<Token> {
    let mut tokens = Vec::new();
    let mut chars = input.chars().peekable();
    while let Some(c) = chars.next() {
        match c {
            ' ' | '\t' | '\n' => {}
            '0'..='9' | '.' => {
                let mut texto = String::from(c);
                while let Some(&p) = chars.peek() {
                    if p.is_ascii_digit() || p == '.' {
                        texto.push(chars.next().unwrap());
                    } else {
                        break;
                    }
                }
                if let Ok(n) = texto.parse::<f64>() {
                    tokens.push(Token::Numero(n));
                }
            }
            '+' | '-' | '*' | '/' => tokens.push(Token::Operador(c)),
            '(' | ')' => tokens.push(Token::Parenteses(c)),
            _ if c.is_alphabetic() || c == '_' => {
                let mut nome = String::from(c);
                while let Some(&p) = chars.peek() {
                    if p.is_alphanumeric() || p == '_' {
                        nome.push(chars.next().unwrap());
                    } else {
                        break;
                    }
                }
                tokens.push(Token::Identificador(nome));
            }
            _ => {}
        }
    }
    tokens
}

fn parsear_int(s: &str) -> Result<i32, String> {
    s.parse::<i32>().map_err(|_| String::from("nao e um numero"))
}

fn dividir(a: i32, b: i32) -> Result<i32, String> {
    if b == 0 {
        Err(String::from("divisao por zero"))
    } else {
        Ok(a / b)
    }
}

fn pipeline(s: &str) -> Result<i32, String> {
    let n = parsear_int(s)?;
    dividir(n, 2)
}

fn main() {
    let r = Retangulo::quadrado(5.0);
    println!("area={} perimetro={} contem={:?}", r.area(), r.perimetro(),
             r.pode_conter(&Retangulo { largura: 3.0, altura: 4.0 }));
    println!("forma circulo: {:.2}", area(&Forma2D::Circulo(1.0)));
    println!("forma retangulo: {:.1}", area(&Forma2D::Retangulo { largura: 3.0, altura: 4.0 }));
    println!("forma triangulo: {:.1}", area(&Forma2D::Triangulo { base: 6.0, altura: 2.0 }));

    let encadeado = Some(20).and_then(|v| dividir_se_positivo(v, 3)).map(|v| v * 2);
    println!("encadeado: {:?}", encadeado); // Some(12)

    let lista = Lista::Cons(1, Box::new(Lista::Cons(2, Box::new(Lista::Cons(3, Box::new(Lista::Nil))))));
    println!("lista: comp={} soma={} ultimo={:?}", comprimento(&lista), soma(&lista), ultimo(&lista));

    println!("tokens: {:?}", tokenizar("3.14 + 2 * (x - 1)"));
    println!("pipeline '21': {:?} / pipeline 'x': {:?}", pipeline("21"), pipeline("x"));
    assert_eq!(pipeline("21"), Ok(10));
    assert!(pipeline("x").is_err());
}
```

A lista com `Box` e o primeiro desenho recursivo honesto: tamanhos sao conhecidos porque cada `Cons` guarda o valor e **um ponteiro** para o resto — sem o `Box`, o tipo teria tamanho infinito e nao compila. O tokenizer deste laboratorio ignora caracteres desconhecidos; um tokenizer de producao devolveria erro posicional em vez de descartar — declare essa politica no seu.

**Autoverificacao:** `encadeado: Some(12)`, lista com comprimento 3/soma 6/ultimo 3 e tokens na ordem `Numero, Operador, Numero, Operador, Parenteses, Identificador, Operador, Numero, Parenteses`.
