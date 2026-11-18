---
layout: default
---

# Aula 23e: pistas graduais

Volte aos [exercícios](../23e_rpc_sobre_consenso.md#10-exercícios-obrigatórios) antes de abrir as soluções.

1. O `for` de `servir` compara só `cache[i].id`. O argumento novo nem é lido quando o `id` já existe: a resposta volta 42 com `respondido=1`. Pense em quem paga quando o cliente reutiliza `id` para um pedido diferente — idempotência exige `id` novo por *intenção* nova.
2. Sem a guarda, todo pedido sem `id` tem `strcmp(cache[i].id, "")` — o primeiro vazio gravado vira dono de todos os vazios seguintes, ou cada um executa de novo se você também pular a gravação. Conte `execucoes` nos dois casos e diga qual é pior: resposta errada compartilhada ou efeito colateral repetido.
3. O 17º `id` não cabe em `MAX_PEDIDOS 16`. O `assert` aborta o servidor inteiro por um cliente a mais — disponibilidade trocada por preguiça. Retorne `-2` antes do `assert`: o pedido falha, o servidor vive, o cache segue válido. Teste os 16 antigos depois.
4. `strncpy(chave, rede, 31)` de dois `id`s que só diferem no 32º caractere produz a mesma chave — e `strcmp` confirma a igualdade falsa. A correção mora na porta: `strlen(id) >= 32` rejeita antes de comparar. IDs de rede têm tamanho máximo declarado, como todo campo de protocolo.
