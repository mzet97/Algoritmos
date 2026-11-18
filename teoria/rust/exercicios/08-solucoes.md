---
layout: default
---

# Aula 08 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../08.md#13-exercícios-obrigatórios).

```rust
use std::fmt;
use std::num::ParseIntError;

fn dividir(a: i32, b: i32) -> Result<i32, String> {
    if b == 0 {
        Err(String::from("divisao por zero"))
    } else {
        Ok(a / b)
    }
}

#[derive(Debug)]
struct Config {
    host: String,
    porta: u16,
    debug: bool,
}

#[derive(Debug)]
enum ErroConfig {
    Formato(String),
    Parse(String),
    CampoAusente(String),
}

impl fmt::Display for ErroConfig {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            ErroConfig::Formato(m) => write!(f, "formato: {}", m),
            ErroConfig::Parse(m) => write!(f, "parse: {}", m),
            ErroConfig::CampoAusente(m) => write!(f, "campo ausente: {}", m),
        }
    }
}
impl std::error::Error for ErroConfig {}

fn parsear_config(texto: &str) -> Result<Config, ErroConfig> {
    let mut host: Option<String> = None;
    let mut porta: Option<u16> = None;
    let mut debug: Option<bool> = None;
    for parte in texto.split(';') {
        let (chave, valor) = parte
            .split_once('=')
            .ok_or_else(|| ErroConfig::Formato(parte.to_string()))?;
        match chave {
            "host" => host = Some(valor.to_string()),
            "porta" => {
                porta = Some(
                    valor
                        .parse::<u16>()
                        .map_err(|_| ErroConfig::Parse(format!("porta={}", valor)))?,
                )
            }
            "debug" => {
                debug = Some(match valor {
                    "true" => true,
                    "false" => false,
                    _ => return Err(ErroConfig::Parse(format!("debug={}", valor))),
                })
            }
            outro => return Err(ErroConfig::Formato(format!("chave {}", outro))),
        }
    }
    Ok(Config {
        host: host.ok_or_else(|| ErroConfig::CampoAusente(String::from("host")))?,
        porta: porta.ok_or_else(|| ErroConfig::CampoAusente(String::from("porta")))?,
        debug: debug.unwrap_or(false),
    })
}

#[derive(Debug)]
enum ErroApp {
    Io(std::io::Error),
    Parse(ParseIntError),
}

impl From<std::io::Error> for ErroApp {
    fn from(e: std::io::Error) -> ErroApp {
        ErroApp::Io(e)
    }
}
impl From<ParseIntError> for ErroApp {
    fn from(e: ParseIntError) -> ErroApp {
        ErroApp::Parse(e)
    }
}
impl fmt::Display for ErroApp {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            ErroApp::Io(e) => write!(f, "io: {}", e),
            ErroApp::Parse(e) => write!(f, "parse: {}", e),
        }
    }
}
impl std::error::Error for ErroApp {}

fn ler_numeros_de_arquivo(caminho: &str) -> Result<Vec<i32>, ErroApp> {
    let texto = std::fs::read_to_string(caminho)?;
    texto
        .split(',')
        .map(|p| p.trim().parse::<i32>().map_err(ErroApp::from))
        .collect()
}

fn retry<F, T, E>(mut f: F, tentativas: u32) -> Result<T, E>
where
    F: FnMut() -> Result<T, E>,
{
    let mut ultimo_erro: Option<E> = None;
    for _ in 0..tentativas.max(1) {
        match f() {
            Ok(v) => return Ok(v),
            Err(e) => ultimo_erro = Some(e),
        }
    }
    Err(ultimo_erro.expect("ao menos uma tentativa executou"))
}

#[derive(Debug)]
enum ErroConversao {
    FormatoInvalido,
    EscalaDesconhecida(String),
}

fn converter(input: &str) -> Result<f64, ErroConversao> {
    let (numero, escala) = input.split_at(input.len().saturating_sub(1));
    let valor: f64 = numero.parse().map_err(|_| ErroConversao::FormatoInvalido)?;
    match escala {
        "C" => Ok(valor * 9.0 / 5.0 + 32.0),
        "F" => Ok((valor - 32.0) * 5.0 / 9.0),
        outra => Err(ErroConversao::EscalaDesconhecida(outra.to_string())),
    }
}

#[derive(Debug)]
struct Usuario {
    nome: String,
    email: String,
    idade: u32,
}

fn validar_usuario(nome: &str, email: &str, idade: &str) -> Result<Usuario, Vec<String>> {
    let mut erros: Vec<String> = Vec::new();
    if nome.trim().is_empty() {
        erros.push(String::from("nome vazio"));
    }
    if !email.contains('@') {
        erros.push(String::from("email sem @"));
    }
    let idade_ok = match idade.parse::<u32>() {
        Ok(v) if v <= 130 => Some(v),
        _ => {
            erros.push(String::from("idade invalida"));
            None
        }
    };
    if erros.is_empty() {
        Ok(Usuario {
            nome: nome.to_string(),
            email: email.to_string(),
            idade: idade_ok.unwrap_or(0),
        })
    } else {
        Err(erros)
    }
}

fn main() {
    for (a, b) in [(10, 2), (7, 0), (9, 3)] {
        match dividir(a, b) {
            Ok(v) => println!("{}/{} = {}", a, b, v),
            Err(e) => println!("{}/{} -> erro: {}", a, b, e),
        }
    }

    match parsear_config("host=localhost;porta=8080;debug=true") {
        Ok(c) => println!("config: {}:{} debug={}", c.host, c.porta, c.debug),
        Err(e) => println!("config erro: {}", e),
    }
    println!("config sem host: {:?}", parsear_config("porta=80").unwrap_err());

    std::fs::write("numeros.txt", "1, 2, 3").expect("escreve fixture");
    println!("arquivo: {:?}", ler_numeros_de_arquivo("numeros.txt"));
    println!("arquivo ausente: {:?}", ler_numeros_de_arquivo("nao_existe.txt").unwrap_err());
    std::fs::remove_file("numeros.txt").ok();

    let valores: Vec<Option<i32>> = vec![Some(3), None, Some(7), Some(5)];
    let soma: i32 = valores.iter().filter_map(|o| *o).sum();
    let maior = valores.iter().filter_map(|o| *o).max();
    println!("some={} maior={:?}", soma, maior);

    let mut tentativas = 0;
    let resultado: Result<i32, String> = retry(
        || {
            tentativas += 1;
            if tentativas < 3 {
                Err(String::from("ainda nao"))
            } else {
                Ok(7)
            }
        },
        5,
    );
    println!("retry: {:?} em {} tentativas", resultado, tentativas);

    println!("100C -> {:?}", converter("100C"));
    println!("212F -> {:?}", converter("212F"));
    match converter("100K") {
        Err(ErroConversao::EscalaDesconhecida(escala)) => {
            println!("100K -> escala desconhecida: {}", escala)
        }
        outro => println!("100K -> inesperado: {:?}", outro),
    }
    match validar_usuario("", "semarroba", "abc") {
        Ok(_) => println!("usuario ok (inesperado)"),
        Err(erros) => println!("erros coletados: {:?}", erros),
    }
    match validar_usuario("Ana", "ana@x.dev", "30") {
        Ok(u) => println!("usuario ok: {} {} {}", u.nome, u.email, u.idade),
        Err(e) => println!("usuario inesperadamente invalido: {:?}", e),
    }
}
```

Dois pontos que o enunciado deixa em aberto e esta solucao fixa: `converter` de entrada vazia cai em `FormatoInvalido` pelo parse falhar, e `debug` ausente no config vale `false` enquanto `host`/`porta` ausentes sao erro — politicas declaradas, nao acidentes. O coletor de erros so e `Ok` com **zero** erros; validar tudo antes de decidir e o que diferencia de uma cadeia de `?`.

**Autoverificacao:** a divisao por zero aparece como valor tratado (nao panic), `retry` para no primeiro `Ok` e o usuario invalido devolve os tres erros de uma vez.
