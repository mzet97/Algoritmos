---
layout: default
---

# Soluções comentadas da aula 16a

## 1. Caminho do `read` em `/dev/null`

Chamada → troca para modo kernel → driver do dispositivo nulo retorna 0 bytes imediatamente → troca de volta. Não há disco, rede nem espera: por isso a régua mede a travessia mínima real (diferente do `getpid` via vDSO das trilhas C/C++, que pode nem trocar de modo).

## 2. Passos 1, 8 e 64

Passo 1 usa todas as palavras de cada linha de 64 bytes; passo 8 usa uma por linha (8x mais linhas tocadas); passo 64 toca uma palavra a cada 8 linhas, e o prefetcher de hardware pode carregar linhas à frente — às vezes o passo 64 "ganha" do passo 8 por tocar menos no total. Moral: a curva real mistura localidade, prefetch e pressão no TLB; relate os três números, não só o vencedor.

## 3. Código 3 e morte por sinal

```rust
let e = Command::new("/bin/sh").arg("-c").arg("exit 3").status().unwrap();
assert_eq!(e.code(), Some(3));
```

Se o filho morre por sinal (ex.: `kill -SEGV`), `code()` devolve `None` — o tipo `Option<i32>` existe exatamente para esse caso. Em C, a mesma informação exige `WIFEXITED`/`WIFSIGNALED`.

## 4. 100 MB e o que volta

```rust
let antes = rss_kb();
let mut bloco = vec![0u8; 100 << 20];
for i in (0..bloco.len()).step_by(4096) {
    bloco[i] = 1;
}
let depois = rss_kb();
drop(bloco);
let depois_drop = rss_kb();
```

`depois > antes` (páginas materializadas); após `drop`, o RSS raramente volta ao valor inicial: o alocador retém páginas para reuse. `drop` libera para o runtime, não promete devolver ao SO.
