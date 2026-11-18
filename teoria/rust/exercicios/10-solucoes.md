---
layout: default
---

# Aula 10 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../10.md#15-exercícios-obrigatórios).

```rust
fn primeiro<'a, 'b>(x: &'a str, _y: &'b str) -> &'a str {
    x
}

struct Palavra<'a> {
    texto: &'a str,
    inicio: usize,
}

impl<'a> Palavra<'a> {
    fn nova(texto: &'a str) -> Palavra<'a> {
        Palavra { texto, inicio: 0 }
    }
    fn comprimento(&self) -> usize {
        self.texto.chars().count()
    }
    fn eh_maiuscula(&self) -> bool {
        self.texto.chars().next().map(|c| c.is_uppercase()).unwrap_or(false)
    }
    fn fatia(&self) -> &'a str {
        &self.texto[self.inicio..]
    }
}

struct PalavraOwned {
    texto: String,
    inicio: usize,
}

impl PalavraOwned {
    fn nova(texto: &str) -> PalavraOwned {
        PalavraOwned { texto: texto.to_string(), inicio: 0 }
    }
    fn comprimento(&self) -> usize {
        self.texto.chars().count()
    }
    fn fatia(&self) -> &str {
        &self.texto[self.inicio..]
    }
}

struct Tokenizer<'a> {
    input: &'a str,
    pos: usize,
}

impl<'a> Tokenizer<'a> {
    fn novo(input: &'a str) -> Tokenizer<'a> {
        Tokenizer { input, pos: 0 }
    }
    fn proximo(&mut self) -> Option<&'a str> {
        let resto = &self.input[self.pos..];
        let aparado = resto.trim_start();
        if aparado.is_empty() {
            return None;
        }
        let consumidos_espaco = resto.len() - aparado.len();
        let fim = aparado.find(char::is_whitespace).unwrap_or(aparado.len());
        let token = &aparado[..fim];
        self.pos += consumidos_espaco + fim;
        Some(token)
    }
}

fn prefixo_comum<'a>(a: &'a str, b: &'a str) -> &'a str {
    let mut fim = 0;
    for (ca, cb) in a.chars().zip(b.chars()) {
        if ca != cb {
            break;
        }
        fim += ca.len_utf8();
    }
    &a[..fim]
}

struct Cache<'a, T> {
    valor: Option<&'a T>,
}

impl<'a, T> Cache<'a, T> {
    fn novo() -> Cache<'a, T> {
        Cache { valor: None }
    }
    fn definir(&mut self, val: &'a T) {
        self.valor = Some(val);
    }
    fn obter(&self) -> Option<&'a T> {
        self.valor
    }
    fn invalidar(&mut self) {
        self.valor = None;
    }
}

fn main() {
    let x = String::from("curto");
    let resultado;
    {
        let y = String::from("y");
        resultado = primeiro(&x, &y).to_string();
    }
    println!("primeiro vive de x: {}", resultado);

    let texto = String::from("Ola mundo");
    let palavra = Palavra::nova(&texto);
    println!("palavra: '{}' comp={} maiuscula={}",
             palavra.fatia(), palavra.comprimento(), palavra.eh_maiuscula());

    let mut tk = Tokenizer::novo("a b c d");
    let mut tokens = Vec::new();
    while let Some(t) = tk.proximo() {
        tokens.push(t);
    }
    println!("tokens: {:?}", tokens);
    assert_eq!(tokens, vec!["a", "b", "c", "d"]);

    let owned = PalavraOwned::nova("independente");
    println!("owned comp={} fatia='{}'", owned.comprimento(), owned.fatia());

    assert_eq!(prefixo_comum("abacate", "aba"), "aba");
    assert_eq!(prefixo_comum("casa", "carro"), "ca");
    println!("prefixo: {}", prefixo_comum("abacate", "abacaxi"));

    let numero = 42;
    let mut cache: Cache<i32> = Cache::novo();
    cache.definir(&numero);
    println!("cache: {:?}", cache.obter());
    cache.invalidar();
    println!("cache apos invalidar: {:?}", cache.obter());
}
```

4. Referencias (`Palavra<'a>`) nao alocam e nao copiam, mas amarram a struct ao texto: ela nao pode ser devolvida para fora do escopo do texto nem guardada num lugar que viva mais. Owned (`PalavraOwned`) paga uma alocacao e passa a viver por conta propria — melhor quando o dado precisa atravessar fronteiras de escopo, thread ou tempo; pior quando e so uma visao temporaria sobre um buffer que ja existe. A escolha e sobre **quem vive quanto**, nao sobre performance isolada.

7. (Explain the error) Os trechos classicos desta aula falham por: devolver referencia para variavel local (o dono morre no fim da funcao), segurar `&mut` enquanto existe `&` vivo do mesmo dado, e usar um valor depois de movido para uma struct que o guarda por referencia sem o lifetime declarado. Em todos, a correcao e declarar a relacao real (lifetime, owned ou escopo menor) — nunca "enganar" o compilador.

**Autoverificacao:** tokens `a b c d` coletados do input original sem alocar, prefixo `"aba"` e o cache incapaz de ser usado apos a saida de escopo do valor (tente e leia o erro).
