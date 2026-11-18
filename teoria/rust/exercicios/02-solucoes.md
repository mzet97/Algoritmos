---
layout: default
---

# Aula 02 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../02.md#13-exercícios-obrigatórios).

```rust
fn fibonacci(n: u32) -> u64 {
    let (mut a, mut b) = (0u64, 1u64);
    for _ in 0..n {
        (a, b) = (b, a + b);
    }
    a
}

fn potencia(base: i64, mut exp: u32) -> i64 {
    let mut resultado: i64 = 1;
    let mut b = base;
    while exp > 0 {
        if exp % 2 == 1 {
            resultado *= b;
        }
        exp /= 2;
        if exp > 0 {
            b *= b;
        }
    }
    resultado
}

fn tabela(n: i32) {
    for i in 1..=10 {
        println!("{} x {} = {}", n, i, n * i);
    }
}

fn eh_primo(n: u32) -> bool {
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

fn main() {
    for n in 1..=100 {
        if n % 15 == 0 {
            println!("FizzBuzz");
        } else if n % 3 == 0 {
            println!("Fizz");
        } else if n % 5 == 0 {
            println!("Buzz");
        } else {
            println!("{}", n);
        }
    }

    assert_eq!(fibonacci(10), 55);
    assert_eq!(fibonacci(20), 6765);
    assert_eq!(potencia(2, 10), 1024);
    assert_eq!(potencia(3, 0), 1);
    assert_eq!(potencia(5, 3), 125);
    tabela(7);
    assert!(eh_primo(2) && eh_primo(7) && eh_primo(97));
    assert!(!eh_primo(10) && !eh_primo(1));
    println!("testes ok");
}
```

6. Um bloco em Rust e uma **expressao**: seu valor e o da ultima expressao **sem** ponto e virgula. Com `a + b;`, a ultima linha vira uma declaracao de efeito descartado e o bloco vale `()` — e atribuir `()` a `x` para imprimir como numero nao compila. O ponto e virgula transforma valor em descarte; e por isso funcoes "devolvem" sem a palavra `return` na ultima linha.

A guarda `if exp > 0` antes de elevar ao quadrado evita um quadrado final inutil (e um overflow desnecessario em bases grandes); o invariante e `resultado * b^exp` constante a cada volta.

**Autoverificacao:** FizzBuzz com o caso 15 primeiro, asserts de Fibonacci/potencia/primo verdes e a tabuada com range inclusivo.
