use bib1::Leitor;

struct No {
    op: char,
    e: i32,
    esq: Option<Box<No>>,
    dir: Option<Box<No>>,
}

fn expressao_para_arvore(c: &[char], ini: usize, fim: usize) -> Option<Box<No>> {
    if ini > fim {
        return None;
    }
    // Find the outermost operator at the shallowest depth
    let mut depth = 0;
    let mut op_pos = None;
    let mut i = ini;
    while i <= fim {
        if c[i] == '(' {
            depth += 1;
        } else if c[i] == ')' {
            depth -= 1;
        } else if depth == 0 && (c[i] == '+' || c[i] == '-' || c[i] == '*' || c[i] == '/') {
            op_pos = Some(i);
            break;
        }
        i += 1;
    }
    match op_pos {
        Some(pos) => {
            let mut node = Box::new(No { op: c[pos], e: 0, esq: None, dir: None });
            // left: skip outer parens if present
            let (l_ini, l_fim) = if c[ini] == '(' {
                (ini + 1, pos - 2)
            } else {
                (ini, pos - 1)
            };
            let (r_ini, r_fim) = if c[pos + 1] == '(' {
                (pos + 2, fim - 1)
            } else {
                (pos + 1, fim)
            };
            node.esq = expressao_para_arvore(c, l_ini, l_fim);
            node.dir = expressao_para_arvore(c, r_ini, r_fim);
            Some(node)
        }
        None => {
            // It's a number (possibly wrapped in parens)
            let (real_ini, real_fim) = if c[ini] == '(' && c[fim] == ')' {
                (ini + 1, fim - 1)
            } else {
                (ini, fim)
            };
            let mut val = 0;
            for j in real_ini..=real_fim {
                if c[j] >= '0' && c[j] <= '9' {
                    val = val * 10 + (c[j] as i32 - '0' as i32);
                }
            }
            Some(Box::new(No { op: '@', e: val, esq: None, dir: None }))
        }
    }
}

fn escrever(t: &Option<Box<No>>) {
    if let Some(ref node) = *t {
        escrever(&node.esq);
        escrever(&node.dir);
        if node.op != '@' {
            print!("{} ", node.op);
        } else {
            print!("{} ", node.e);
        }
    }
}

fn main() {
    let mut leitor = Leitor::new();
    while let Some(line) = leitor.entrada() {
        let chars: Vec<char> = line.trim().chars().collect();
        if chars.is_empty() { continue; }
        let t = expressao_para_arvore(&chars, 0, chars.len() - 1);
        escrever(&t);
        println!();
    }
}
