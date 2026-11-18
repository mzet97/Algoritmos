---
layout: default
---

# Aula 28 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../28.md#exercícios-obrigatórios). Projeto: `cargo add memmap2`.

```rust
use std::fs::File;
use std::io::{BufReader, Read};
use std::time::Instant;

fn contar_bufreader(caminho: &str) -> std::io::Result<usize> {
    let mut leitor = BufReader::new(File::open(caminho)?);
    let mut buffer = [0u8; 8192];
    let mut total = 0usize;
    loop {
        let lidos = leitor.read(&mut buffer)?;
        if lidos == 0 {
            break;
        }
        total += buffer[..lidos].iter().filter(|&&b| b == b'A').count();
    }
    Ok(total)
}

fn contar_mmap(caminho: &str) -> std::io::Result<usize> {
    let arquivo = File::open(caminho)?;
    // Safety: o arquivo nao e modificado por ninguem durante a contagem
    // (fixture deste laboratorio); mapear arquivo que pode ser truncado
    // por outro processo seria outro contrato.
    let mapa = unsafe { memmap2::Mmap::map(&arquivo)? };
    Ok(mapa.iter().filter(|&&b| b == b'A').count())
}

fn main() -> std::io::Result<()> {
    let caminho = "fixture50.bin";
    {
        let mut arquivo = File::create(caminho)?;
        use std::io::Write;
        let bloco = vec![b'A'; 1_000_000];
        for _ in 0..50 {
            arquivo.write_all(&bloco)?;
        }
    }

    let inicio = Instant::now();
    let via_leitor = contar_bufreader(caminho)?;
    println!("bufreader: {} em {:?}", via_leitor, inicio.elapsed());

    let inicio = Instant::now();
    let via_mapa = contar_mmap(caminho)?;
    println!("mmap: {} em {:?}", via_mapa, inicio.elapsed());

    assert_eq!(via_leitor, 50_000_000);
    assert_eq!(via_mapa, 50_000_000);
    std::fs::remove_file(caminho)?;
    println!("contagens iguais");
    Ok(())
}
```

Nota honesta sobre o exercicio: ler "linha por linha" com `BufRead::lines` sobre um arquivo sem `\n` degenera para uma linha de 50 MB — por isso esta solucao le em blocos com `read`, que e o que `BufReader` faz por baixo; se voce implementar com `lines`, declare a diferenca. Os tempos impressos sao desta maquina: com o arquivo em page cache, o mmap tende a vencer por nao copiar; em disco frio, a primeira leitura paga as faltas de pagina — rode 3 vezes e anote os dois regimes, como na [aula 16 de C](../../c/exercicios/16-solucoes.md).

2. io_uring em duas linhas: em vez de uma syscall por operacao (o modelo `epoll` + `read`/`write`), o programa enfileira descricoes de operacoes num anel compartilhado com o kernel e colhe conclusoes em lote — menos travessias, operacoes encadeaveis (ler→escrever) sem voltar ao usuario e I/O de arquivo de verdade assincrono, coisa que `epoll` nunca fez bem.

**Autoverificacao:** as duas contagens em 50.000.000, o arquivo gerado fora da medicao e os tempos declarados como desta maquina com o regime de cache nomeado.
