use std::io::{self, BufRead, BufReader, Stdin};

pub struct Leitor {
    reader: BufReader<Stdin>,
}

impl Leitor {
    pub fn new() -> Self {
        Self {
            reader: BufReader::new(io::stdin()),
        }
    }

    /// Lê uma linha de stdin ignorando linhas vazias.
    /// Retorna None em EOF.
    pub fn entrada(&mut self) -> Option<String> {
        let mut line = String::new();
        loop {
            line.clear();
            match self.reader.read_line(&mut line) {
                Ok(0) => return None,
                Ok(_) => {
                    let trimmed = line.trim().to_string();
                    if !trimmed.is_empty() {
                        return Some(trimmed);
                    }
                }
                Err(_) => return None,
            }
        }
    }
}

/// Formata f64 no estilo do Python (shortest round-trip representation).
/// Rust já usa shortest-representation com Display; basta garantir ".0" para inteiros.
pub fn python_float(x: f64) -> String {
    if x.is_infinite() {
        return if x < 0.0 { "-inf".into() } else { "inf".into() };
    }
    if x.is_nan() {
        return "nan".into();
    }
    let s = format!("{}", x);
    if !s.contains('.') && !s.contains('e') && !s.contains('E') {
        format!("{}.0", s)
    } else {
        s
    }
}
