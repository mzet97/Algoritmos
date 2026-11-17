use bib1::Leitor;

fn verifica_parentizacao(e: &[(char, i32)]) -> bool {
    let mut stack: Vec<i32> = Vec::new();
    for &(desc, tipo) in e {
        if desc == '(' {
            stack.push(tipo);
        } else {
            if stack.is_empty() {
                return false;
            }
            let t = stack.pop().unwrap();
            if t != tipo {
                return false;
            }
        }
    }
    stack.is_empty()
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let n: i32 = line.trim().parse().unwrap();
        if n >= 0 {
            let Some(s) = leitor.entrada() else { break };
            let chars: Vec<char> = s.chars().collect();
            let mut elems: Vec<(char, i32)> = Vec::new();
            let mut j = 0;
            while j < chars.len() {
                let desc = chars[j];
                j += 1;
                let mut tipo_str = String::new();
                while j < chars.len() && chars[j] != '(' && chars[j] != ')' {
                    tipo_str.push(chars[j]);
                    j += 1;
                }
                let tipo: i32 = if tipo_str.is_empty() {
                    0
                } else {
                    tipo_str.parse().unwrap()
                };
                elems.push((desc, tipo));
            }
            println!("{}", if verifica_parentizacao(&elems) { 1 } else { 0 });
        } else {
            let nlin = n;
            let n = 1000000;
            let mut elems: Vec<(char, i32)> = Vec::new();
            if nlin == -1 {
                for i in 0..n / 2 {
                    elems.push(('(', 1));
                }
                for _ in 0..n / 2 {
                    elems.push((')', 1));
                }
            } else if nlin == -2 {
                for i in 0..n / 2 {
                    elems.push(('(', (i + 1) as i32));
                }
                for i in n / 2..n {
                    elems.push((')', (n - (i + 1) + 1) as i32));
                }
            } else if nlin == -3 {
                for i in (0..n).step_by(2) {
                    elems.push(('(', (i + 1) as i32));
                    elems.push((')', (i + 1) as i32));
                }
            } else if nlin == -4 {
                for _ in 0..(n / 2 - 1) {
                    elems.push(('(', 1));
                }
                for _ in (n / 2 - 1)..n {
                    elems.push((')', 1));
                }
            } else if nlin == -5 {
                for _ in 0..(n / 2 + 1) {
                    elems.push(('(', 1));
                }
                for _ in (n / 2 + 1)..n {
                    elems.push((')', 1));
                }
            }
            println!("{}", if verifica_parentizacao(&elems) { 1 } else { 0 });
        }
    }
}
