use bib1::Leitor;

struct Fila {
    p1: Vec<i32>,
    p2: Vec<i32>,
}

impl Fila {
    fn new() -> Self {
        Fila {
            p1: Vec::new(),
            p2: Vec::new(),
        }
    }

    fn enfileira(&mut self, x: i32) {
        self.p1.push(x);
    }

    fn desenfileira(&mut self) -> i32 {
        if self.p2.is_empty() {
            while let Some(v) = self.p1.pop() {
                self.p2.push(v);
            }
        }
        self.p2.pop().unwrap()
    }

    fn proximo(&mut self) -> i32 {
        if self.p2.is_empty() {
            while let Some(v) = self.p1.pop() {
                self.p2.push(v);
            }
        }
        *self.p2.last().unwrap()
    }

    fn tamanho(&self) -> i32 {
        (self.p1.len() + self.p2.len()) as i32
    }
}

fn main() {
    let mut leitor = Leitor::new();
    let mut f = Fila::new();
    while let Some(line) = leitor.entrada() {
        let parts: Vec<&str> = line.split_whitespace().collect();
        if parts[0] == "i" {
            let e: i32 = parts[1].parse().unwrap();
            f.enfileira(e);
        } else if parts[0] == "r" {
            println!("{}", f.desenfileira());
        } else if parts[0] == "p" {
            println!("{}", f.proximo());
        } else if parts[0] == "n" {
            println!("{}", f.tamanho());
        } else {
            // automatic test
            for i in 1..=1000 {
                f.enfileira(i);
            }
            while f.tamanho() > 0 {
                print!("{} ", f.tamanho());
                print!("{} ", f.proximo());
                print!("{} ", f.desenfileira());
            }
            println!();
        }
    }
}
