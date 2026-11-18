---
layout: default
---

# Projetos Cargo das aulas 30–32 de Rust

Fontes extraídas dos blocos de código das aulas e executadas em ambiente
limpo. A verificação completa (build, rede, concorrência, TTL) roda com:

```bash
python3 teoria/verificar_projetos.py
```

| Projeto | Aula | O que é testado |
| --- | --- | --- |
| `30_load_balancer` | [Aula 30](../30.md) | Proxy TCP Tokio na porta 8080 com round-robin entre backends 8081–8083 (sequência cíclica; a conexão de sondagem também consome um índice) |
| `31_mini_redis` | [Aula 31](../31.md) | `PING/SET/GET/DEL`, chave ausente, comando desconhecido e 8 clientes concorrentes em `127.0.0.1:6379` |
| `32_mini_redis_avancado` | [Aula 32 bônus](../32_bonus.md) | `INCR` atômico por chave (2 clientes × 250 = 500 sem perda), erro em texto e em overflow de `i64`, TTL preguiçoso com `EX1` |

Limites honestos: os servidores das aulas 31–32 continuam protótipos —
parser sem delimitação de comandos parciais, sem limite de tamanho e sem
autenticação; escutam só em `127.0.0.1`. A aula 30 fixa a porta 8080; em
máquinas onde ela está ocupada, o verificador roda o teste em namespace
de rede próprio (`unshare -rn`) sem alterar o código da aula.
