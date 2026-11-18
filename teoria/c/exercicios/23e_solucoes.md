---
layout: default
---

# Aula 23e: soluções comentadas

Compare após tentar os [exercícios](../23e_rpc_sobre_consenso.md#10-exercícios-obrigatórios). Experimentos sobre `teoria/c/exemplos/23e_rpc.c` com `gcc -std=c11 -Wall -Wextra -Wpedantic -Werror`.

1. Previsão: volta **42**, com `respondido=1` e `execucoes` parado em 1 (verificado). O `for` de `servir` compara só o `id`; o argumento novo nunca é lido. Idempotência é por *intenção numerada*: o cliente que recicla `id` para um pedido diferente recebe a resposta do pedido antigo. Regra de bolso: `id` novo por intenção nova, nunca por conveniência.

2. Diagnóstico: sem a guarda, o pedido de `id` vazio deixa de retornar `-1` — é executado, gravado, e o segundo pedido vazio volta do cache do primeiro (o `assert(... == -1)` do `main` cai, provando a mudança). Duas intenções distintas passam a compartilhar uma resposta. "Sem `id`, sem resposta" é a trava que impede essa colisão na porta.

3. Projeto: o (`MAX_PEDIDOS`+1)-ésimo `id` distinto aborta o servidor em `assert(n_cache < MAX_PEDIDOS)` (verificado — no `main` do exemplo, que já ocupa 2 slots, o 15º novo cai). A correção troca o abort por erro explícito:

```c
if (n_cache >= MAX_PEDIDOS)
    return -2; /* cache cheio: o pedido falha, o servidor vive */
assert(n_cache < MAX_PEDIDOS); /* agora e documentacao, nao controle */
```

Depois da troca, os 16 primeiros continuam servindo do cache e só o 17º falha. Abortar o processo por um cliente a mais é trocar disponibilidade por preguiça; em produção, aqui entraria TTL ou despejo — mas nunca o `abort` silencioso.

4. Previsão: sim, colidem. Dois `id`s que só diferem após o 31º caractere, copiados com truncamento para `char[32]`, viram a mesma chave (verificado: `iguais=1`). A correção mora na porta de `servir`, antes de qualquer comparação:

```c
if (strlen(pedido->id) >= (sizeof pedido->id))
    return -1; /* id nao cabe no campo: rejeita, nao trunca */
```

Todo campo de protocolo tem tamanho máximo declarado; aceitar além dele e truncar é fabricar colisão. Note que o literal longo nem compila no cliente (`initializer-string too long` com `-Werror`) — o perigo real é o `id` que chega da rede num buffer maior.
