---
layout: default
---

# Aula 06 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../06.md#16-exercícios-obrigatórios). Projeto Cargo completo — crie com `cargo new aula06` e distribua os blocos:

```rust
// src/lib.rs
pub mod geometria;
pub mod texto;
```

```rust
// src/geometria/mod.rs
pub mod circulo;
pub mod retangulo;

pub use circulo::area as area_circulo;
pub use retangulo::Retangulo;
```

```rust
// src/geometria/circulo.rs
pub fn area(raio: f64) -> f64 {
    std::f64::consts::PI * raio * raio
}

pub fn circunferencia(raio: f64) -> f64 {
    2.0 * std::f64::consts::PI * raio
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn area_raio5() {
        assert!((area(5.0) - 78.54).abs() < 0.01);
    }
}
```

```rust
// src/geometria/retangulo.rs
pub struct Retangulo {
    pub largura: f64,
    pub altura: f64,
}

impl Retangulo {
    pub fn area(&self) -> f64 {
        self.largura * self.altura
    }
    pub fn perimetro(&self) -> f64 {
        2.0 * (self.largura + self.altura)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn perimetro_3x4() {
        let r = Retangulo { largura: 3.0, altura: 4.0 };
        assert_eq!(r.perimetro(), 14.0);
    }
}
```

```rust
// src/texto/mod.rs
pub mod formatador;

pub use formatador::formatar_numero;
```

```rust
// src/texto/formatador.rs
pub fn formatar_numero(n: f64, decimais: usize) -> String {
    format!("{:.*}", decimais, n)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn duas_casas() {
        assert_eq!(formatar_numero(3.14159, 2), "3.14");
    }
}
```

```rust
// src/main.rs
use aula06::geometria::{area_circulo, Retangulo};
use aula06::texto::formatar_numero;

fn main() {
    let r = Retangulo { largura: 3.0, altura: 4.0 };
    println!("circulo: {}", formatar_numero(area_circulo(5.0), 2));
    println!("retangulo: area={} perimetro={}", r.area(), r.perimetro());
}
```

```rust
// Exercicio 2 (modulo banco, em src/banco.rs ou no main de um binario de estudo)
pub struct Conta {
    pub titular: String,
    saldo: f64,
}

impl Conta {
    pub fn nova(titular: String) -> Conta {
        Conta { titular, saldo: 0.0 }
    }
    pub fn depositar(&mut self, valor: f64) {
        self.saldo += valor;
    }
    pub fn sacar(&mut self, valor: f64) -> bool {
        if !validar_saque(self.saldo, valor) {
            return false;
        }
        self.saldo -= valor;
        true
    }
    pub fn ver_saldo(&self) -> f64 {
        self.saldo
    }
}

fn validar_saque(saldo: f64, valor: f64) -> bool {
    valor > 0.0 && valor <= saldo
}
```

Verificado nesta rodada com `cargo test` (3 testes verdes) e `cargo run` imprimindo `circulo: 78.54` e `retangulo: area=12 perimetro=14`. A re-exportacao do exercicio 3 e a linha `pub use circulo::area as area_circulo;`: quem usa escreve `geometria::area_circulo`, sem conhecer a arvore interna. Tentar `conta.saldo` fora do modulo da o erro E0616 (campo privado) — esse erro **e** o exercicio 2 funcionando.

**Autoverificacao:** `cargo test` verde nos tres modulos, `main` usando o caminho re-exportado e o erro de campo privado reproduzido uma vez e depois removido.
