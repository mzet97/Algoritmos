use bib1::Leitor;

fn eh_palindromo(l: &str) -> bool {
    let chars: Vec<char> = l.chars().filter(|&c| c != ' ').collect();
    let n = chars.len();
    for i in 0..n / 2 {
        if chars[i] != chars[n - 1 - i] {
            return false;
        }
    }
    true
}

fn main() {
    let mut leitor = Leitor::new();
    loop {
        let l_raw = match leitor.entrada() {
            Some(s) => s,
            None => break,
        };
        let l = if l_raw == "@entradagrande1" {
            "ab".repeat(500000) + "a"
        } else if l_raw == "@entradagrande2" {
            "ab".repeat(500000)
        } else {
            l_raw
        };
        println!("{}", if eh_palindromo(&l) { "True" } else { "False" });
    }
}
