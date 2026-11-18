---
layout: default
---

# Aula 31 (Rust): solucoes comentadas

Compare apos tentar os [exercicios](../31.md#checklist). Estado executado: o mini-Redis da aula virou o projeto [31_mini_redis](../projetos/README.md); o `teoria/verificar_projetos.py` sobe o servidor real e confirma `PING`, `SET`, `GET`, chave ausente (`$-1`), `DEL` (`:1`/`:0`), comando desconhecido (`-ERR`) e 8 clientes concorrentes com chaves proprias.

O roteiro manual do checklist, para voce reproduzir com o servidor do projeto rodando (`cargo run` em `projetos/31_mini_redis`):

```text
$ nc 127.0.0.1 6379
SET nome Engenheiro
+OK
GET nome
+Engenheiro
DEL nome
:1
GET nome
$-1
FOO x
-ERR Comando desconhecido: FOO
```

O que cada resposta ensina: `+` e string simples, `:` e inteiro e `$-1` e bulk string nula do protocolo RESP (o formato que a [aula 32](32-solucoes.md) estende). O `DEL` em chave que nao existe devolve `:0` e nao erro — e assim que o Redis real se comporta, e a politica fica visivel na resposta, nao num log.

Limites que o checklist manda internalizar: parser sem enquadramento de comandos parciais (uma linha partida em dois pacotes pode ser lida como dois comandos), sem limite de tamanho e sem autenticacao. E um prototipo de laboratorio em `127.0.0.1`, nao um servidor para expor — o degrau de endurecimento e a [aula 23a de C++](../../cpp/23a.md) aplicada a este desenho.

**Autoverificacao:** o roteiro manual reproduzido por voce, dois clientes simultaneos sem interferencia e os limites do prototipo ditos em voz alta antes de alguem propor "colocar no ar".
