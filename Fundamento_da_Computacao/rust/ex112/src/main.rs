use bib1::Leitor;
use std::collections::HashMap;

fn main() {
    let mut dist: HashMap<&str, f64> = HashMap::new();
    dist.insert("km", 1000.0); dist.insert("hm", 100.0); dist.insert("dam", 10.0);
    dist.insert("m", 1.0); dist.insert("dm", 0.1); dist.insert("cm", 0.01); dist.insert("mm", 0.001);
    let mut mass: HashMap<&str, f64> = HashMap::new();
    mass.insert("kg", 1000.0); mass.insert("hg", 100.0); mass.insert("dag", 10.0);
    mass.insert("g", 1.0); mass.insert("dg", 0.1); mass.insert("cg", 0.01); mass.insert("mg", 0.001);
    let mut vol: HashMap<&str, f64> = HashMap::new();
    vol.insert("kl", 1000.0); vol.insert("hl", 100.0); vol.insert("dal", 10.0);
    vol.insert("l", 1.0); vol.insert("dl", 0.1); vol.insert("cl", 0.01); vol.insert("ml", 0.001);

    let mut leitor = Leitor::new();
    loop {
        let x: i64 = match leitor.entrada() {
            Some(s) => match s.parse() { Ok(v) => v, Err(_) => break },
            None => break,
        };
        let u1 = match leitor.entrada() { Some(s) => s, None => break };
        let u2 = match leitor.entrada() { Some(s) => s, None => break };
        let result = (x as f64) * dist.get(u1.as_str()).or_else(|| mass.get(u1.as_str())).or_else(|| vol.get(u1.as_str())).unwrap()
            / dist.get(u2.as_str()).or_else(|| mass.get(u2.as_str())).or_else(|| vol.get(u2.as_str())).unwrap();
        println!("{}", result as i64);
    }
}
