fn propriedade() -> Vec<i64> {
    let mut resultados: Vec<i64> = Vec::new();
    for n in 1000..10000 {
        let esq = n / 100;
        let dir = n % 100;
        if (esq + dir) * (esq + dir) == n {
            resultados.push(n);
        }
    }
    resultados
}

fn main() {
    let r = propriedade();
    println!("{:?}", r);
}
