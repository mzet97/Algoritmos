use bib1::Leitor;

fn eh_primo(x: i64) -> bool {
    if x < 2 {
        return false;
    }
    if x == 2 {
        return true;
    }
    if x % 2 == 0 {
        return false;
    }
    let mut i: i64 = 3;
    while i * i <= x {
        if x % i == 0 {
            return false;
        }
        i += 2;
    }
    true
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let x: i64 = match leitor.entrada() {
            Some(s) => match s.parse() {
                Ok(v) => v,
                Err(_) => break,
            },
            None => break,
        };
        println!("{}", if eh_primo(x) { "True" } else { "False" });
    }
}
