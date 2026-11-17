# Soluções comentadas da aula 14a

## 1. Caminho do `PING` fatiado

Cada `write` de 1 byte vira um segmento (ou poucos); cada `read` do servidor devolve "o que chegou", sem respeitar fronteiras de mensagem. `read_until(b'\n')` acumula em `linha` até completar; só então o servidor responde. Quem mora onde: bytes em voo (buffers do SO, ambos os lados), bytes lidos (buffer do `BufReader`), linha completa (`Vec<u8> linha`).

## 2. Teto de 64 bytes

Passam linhas de até 64 bytes; acima disso, o mesmo erro protocolado. O teste "grande demais" usa 4106 bytes, que continua acima — por isso segue passando. Moral: teste de limite deve cobrir os dois lados da fronteira (64 passa, 65 falha), não um único ponto distante.

## 3. Sem timeout, thread vaza

Sem `set_read_timeout`, o cliente mudo deixa a thread bloqueada em `read_until` para sempre: `lentos` não sobe, a thread não volta ao pool (aqui, não volta nunca), e 32 clientes mudos esgotam o teto. Com `timeout` externo, o processo morre mas o diagnóstico some. Timeout no socket + contador é observabilidade, não gambiarra.

## 4. 40 conexões, 8 recusas

```rust
let mut alvos = Vec::new();
for _ in 0..40 {
    alvos.push(thread::spawn(|| {
        fala(&base, &[b"X\n"])
    }));
}
let mut oks = 0;
for a in alvos {
    if a.join().unwrap() == b"X\n" { oks += 1; }
}
assert!(srv.estado.lock().unwrap().recusadas > 0);
// oks == 32 se todos os rapidos chegarem antes do teto encher
```

Se `oks < 32` sem `recusadas` subir, há condição de corrida no próprio contador — e o teste a denuncia em vez de escondê-la.
