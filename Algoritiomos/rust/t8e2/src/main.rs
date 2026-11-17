use bib1::Leitor;
use std::collections::BinaryHeap;
use std::cmp::Reverse;

struct Mediana {
    ante: BinaryHeap<i32>,         // max-heap: elements <= median
    post: BinaryHeap<Reverse<i32>>, // min-heap: elements > median
    n: i32,
    med: f64,
}

impl Mediana {
    fn new(_n: i32) -> Self {
        Mediana {
            ante: BinaryHeap::new(),
            post: BinaryHeap::new(),
            n: 0,
            med: 0.0,
        }
    }

    fn obter_mediana(&self) -> f64 {
        self.med
    }

    fn insere(&mut self, x: i32) {
        if self.n == 0 {
            self.ante.push(x);
            self.med = x as f64;
        } else if self.n % 2 == 1 {
            // odd -> even: median will be average of two middle
            if (x as f64) <= self.med {
                self.ante.push(x);
            } else {
                self.post.push(Reverse(x));
            }
            // balance: ante should have one more than post for even insertion
            if self.ante.len() > self.post.len() + 1 {
                let v = self.ante.pop().unwrap();
                self.post.push(Reverse(v));
            } else if self.post.len() > self.ante.len() {
                let Reverse(v) = self.post.pop().unwrap();
                self.ante.push(v);
            }
            let a = *self.ante.peek().unwrap() as f64;
            let b = self.post.peek().map(|r| r.0 as f64).unwrap_or(a);
            self.med = (a + b) / 2.0;
        } else {
            // even -> odd: median will be the middle element
            if (x as f64) <= self.med {
                self.ante.push(x);
            } else {
                self.post.push(Reverse(x));
            }
            // balance: ante should have one more than post
            if self.ante.len() > self.post.len() + 1 {
                let v = self.ante.pop().unwrap();
                self.post.push(Reverse(v));
            } else if self.ante.len() < self.post.len() + 1 {
                let Reverse(v) = self.post.pop().unwrap();
                self.ante.push(v);
            }
            self.med = *self.ante.peek().unwrap() as f64;
        }
        self.n += 1;
    }
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n != -1 {
            let mut l: Vec<i32> = Vec::with_capacity(n as usize);
            for _ in 0..n {
                let Some(lin) = leitor.entrada() else { break };
                let e: i32 = lin.trim().parse().unwrap();
                l.push(e);
            }
            let mut m = Mediana::new(n);
            for &x in &l {
                m.insere(x);
                print!("{} ", bib1::python_float(m.obter_mediana()));
            }
            println!();
        } else {
            let n = 100000;
            let l: Vec<i32> = (0..n).collect();
            let mut m = Mediana::new(n);
            for &x in &l {
                m.insere(x);
                print!("{} ", bib1::python_float(m.obter_mediana()));
            }
            println!();
        }
    }
}
