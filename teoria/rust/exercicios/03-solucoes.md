---
layout: default
---

# Aula 03 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../03.md#14-exercícios-obrigatórios).

```rust
fn comprimento(s: &String) -> usize {
    s.len()
}

fn inverter_palavras(s: &mut String) {
    let invertida: String = s.chars().rev().collect();
    *s = invertida;
}

fn criar() -> String {
    String::from("relatorio")
}

fn processar(mut s: String) -> String {
    s.push_str(" processado");
    s
}

fn finalizar(s: &String) {
    println!("final: {}", s);
}

fn primeira_palavra(s: &String) -> usize {
    for (i, &b) in s.as_bytes().iter().enumerate() {
        if b == b' ' {
            return i;
        }
    }
    s.len()
}

fn trocar(a: &mut i32, b: &mut i32) {
    std::mem::swap(a, b);
}

fn main() {
    let texto = String::from("ola");
    let n = comprimento(&texto);
    println!("comprimento {} de '{}'", n, texto); // original intacta

    let mut mutavel = String::from("abc def");
    inverter_palavras(&mut mutavel);
    println!("invertida: {}", mutavel); // fed cba

    let pronta = processar(criar());
    finalizar(&pronta);

    println!("primeira palavra termina em {} bytes", primeira_palavra(&String::from("ola mundo")));
    let mut x = 3;
    let mut y = 9;
    trocar(&mut x, &mut y);
    println!("x={} y={}", x, y); // 9 3
    assert_eq!(x, 9);
    assert_eq!(y, 3);
}
```

4. Move vs Copy, trecho a trecho: **A** compila — `i32` e `Copy`, `y` recebe copia e `x` continua valido. **B** nao compila — `String` nao e `Copy`; `let t = s;` move e `s` fica invalido (erro E0382 em qualquer compilador recente). **C** compila — `clone()` cria uma segunda alocacao, as duas vivem. **D** compila — `&str` (aqui, referencia para o literal no binario) e `Copy` como toda referencia compartilhada: copiar `s` copia o ponteiro+comprimento, e os dados apontados nao tem dono unico para mover.

5. Correcao importante ao enunciado: o indice devolvido e de **byte**. Em `"ola mundo"` (so ASCII ate o espaco), o espaco esta no byte 3. Mas em `"olá mundo"`, o espaco esta no byte **4** (`o`, `l` e os 2 bytes de `á`), nao 3 — fatiar um `&str` em indice de caractere quebraria a codificacao. Quem precisa de contagem de caracteres usa `.chars().count()`/posicoes de `char_indices`, nao indices de corte.

6. Sem `&mut`, a funcao receberia copias dos dois inteiros; trocar copias nao escreve nos objetos do chamador. O `&mut` e a permissao exclusiva e temporaria de escrever **naqueles** objetos.

**Autoverificacao:** a string original imprime depois de `comprimento`, `x=9 y=3` apos `trocar`, e voce consegue dizer o indice de byte do espaco em `"olá mundo"` sem rodar (4).
