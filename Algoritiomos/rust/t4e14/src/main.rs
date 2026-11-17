use bib1::Leitor;

fn eh_palindromo(c: &[char], n: usize) -> bool {
    if n <= 1 {
        true
    } else {
        if c[0] != c[n - 1] {
            false
        } else {
            eh_palindromo(&c[1..n - 1], n - 2)
        }
    }
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n >= 0 {
            let Some(s) = leitor.entrada() else { break };
            let chars: Vec<char> = s.chars().collect();
            println!("{}", if eh_palindromo(&chars, chars.len()) { 1 } else { 0 });
        } else {
            let nlin = n;
            let n = 50000;
            if nlin == -1 {
                let mut c: Vec<char> = Vec::new();
                for i in 0..n {
                    c.push((b'a' + (i % 26) as u8) as char);
                }
                println!("{}", if eh_palindromo(&c, n) { 1 } else { 0 });
            } else {
                let mut c: Vec<char> = vec!['a'; n];
                for i in 0..(n + 1) / 2 {
                    c[i] = (b'a' + (i % 26) as u8) as char;
                    c[n - i - 1] = c[i];
                }
                println!("{}", if eh_palindromo(&c, n) { 1 } else { 0 });
            }
        }
    }
}
