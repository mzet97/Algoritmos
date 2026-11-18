---
layout: default
---

# Aula 08 (Rust): pistas graduais

Volte aos [exercicios](../08.md#13-exercícios-obrigatórios) antes de abrir as solucoes.

1. No `match` das tres divisoes, o braco `Err` tambem e um resultado para imprimir, nao um fim de programa.
2. Parse de config: cada `chave=valor` vira um campo; chave desconhecida ou ausente e um erro nomeado, nao `unwrap`.
3. Com `From`, o `?` converte `io::Error` e `ParseIntError` sem `map_err` no caminho.
4. Sem `match`: `filter_map` sobrevive aos `Some`, `sum` e `max` fecham a conta.
5. `retry` executa, guarda o ultimo erro e so devolve `Err` depois de esgotar as tentativas.
6. Temperatura: o ultimo caractere escolhe a escala; o resto precisa parsear como numero.
7. Coletor de erros valida **tudo** e junta as mensagens — nao retorne no primeiro problema.
