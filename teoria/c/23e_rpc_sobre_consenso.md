---
layout: default
---

# Aula 23e — RPC: o pedido tem nome, a resposta tem memória

**Competências SDD:** DST, NET e SEC. **Pré-requisitos:** aulas [23a](23a_sistemas_distribuidos.md) (transporte e `fsync`), [23c](23c_relogios_e_particao.md) (ordem e conflito) e [23d](23d_consenso_e_quorum.md) (o que vale). O consenso decidiu *o que* conta; esta aula decide *como o cliente vê*: chamar duas vezes não pode executar duas vezes.

**Objetivo observável:** ao terminar, você deverá numerar cada pedido com `id`, servir repetição sem reexecutar (inclusive o erro), e rejeitar pedido sem `id` — explicando por que retry sem `id` é duplicação com passos extras.

**Motivação:** redes repetem. Timeout não diz se o servidor executou e a resposta se perdeu, ou se o pedido nunca chegou. Sem `id`, o cliente que tenta de novo aposta: pode estar pedindo pela primeira ou pela segunda vez sem saber. Com `id`, a aposta some: repetir é pedir a resposta gravada.

## 1. Contrato antes do código

- **Pedido:** `(id, método, argumento)`. Sem `id`, sem resposta — o servidor recusa na porta.
- **Resposta:** `(id, resultado, repetição?)`. `id` novo executa e grava; `id` repetido devolve o gravado com a marca `repetição`.
- **Erro também é resposta:** método inválido grava o erro sob o `id`. Reavaliar o erro a cada retry é reabrir uma decisão tomada.
- **Limite:** 16 pedidos gravados, `id` e método com teto de tamanho. Cache sem teto é vazamento com nome bonito.
- **Fora do contrato:** serialização binária, prazos e cancelamento — o `id` sobrevive a todos eles, mas mora numa camada abaixo.

## 2. Por que o erro também se memoriza?

Porque o cliente repete o erro tanto quanto repete o acerto: o retry não distingue. Se o servidor reexecutar o método inválido a cada repetição, cada retry custa uma decisão nova sobre um fato velho — e um método com efeito colateral parcial pode completar na segunda tentativa o que recusou na primeira. Erro gravado congela a decisão: o pedido morreu uma vez, e morto fica.

## 3. Exemplo completo e executável

O [arquivo-fonte desta aula](exemplos/23e_rpc.c) contém o programa completo:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/23e_rpc.c -o /tmp/23e
/tmp/23e
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-sanitize-recover=all teoria/c/exemplos/23e_rpc.c -o /tmp/23e_asan
/tmp/23e_asan   # identico, sem relatorio de sanitizer
```

Saída esperada (determinística):

```text
rpc req-1 dobrar(21) = 42 (executou)
rpc req-1 repetido = 42 (cache, execucoes=1)
rpc req-2 metodo invalido = erro gravado, repeticao servida do cache
rpc sem id: rejeitado (sem id, sem resposta)
23e ok: idempotencia por id, retry seguro, erro tambem memorizado
```

A prova está no contador `execucoes`: a repetição de `req-1` não o move. Retry seguro não é retry que "dá certo" — é retry que não executa de novo.

### O que a memória faz aqui?

Cache com teto (`MAX_PEDIDOS`, `assert` no estouro): em produção, o teto vira expiração por tempo ou confirmação — o cliente que já leu a resposta não precisa que o servidor a guarde para sempre. `id` copiado com `snprintf` e teto: `id` sem teto é o abuso da 32c de volta.

## 4. Erros comuns e implicações defensivas

1. **Retry sem `id`:** cada tentativa é um pedido novo. Efeito executado duas vezes não é bug da rede — é pedido mal numerado.
2. **Gravar só o acerto:** o erro reavaliado muda de ideia sob carga. Decisão gravada vale para os dois desfechos.
3. **Cache sem teto:** crescimento proporcional ao número de clientes é negação de serviço adiada. Teto e expiração são parte do contrato.
4. **Confundir RPC com consenso:** o RPC entrega a resposta a um cliente; o consenso decide o que vale para todos. Um não substitui o outro — o RPC desta aula roda *sobre* o commit da 23d.
5. **`id` reutilizado para pedido diferente:** o servidor devolve a resposta do pedido antigo. `id` é identidade do pedido, não do cliente — reutilizar é mentir sobre o que se pediu.

## 5. Ponte com as demais trilhas

Go cobra chave de idempotência antes de qualquer retry de escrita; Python dedup por `(autor, seq)` na reconciliação; Rust traduz `id` em tipo que não se confunde. Em JavaScript, o `id` é o `Idempotency-Key` dos gateways de pagamento: o duplo clique no "pagar" é o retry desta aula com roupa de usuário.

## 6. Checklist do que esta aula garante

- [ ] Explico por que timeout não distingue "não executou" de "resposta perdida" — e como o `id` resolve.
- [ ] Demonstro repetição servida do cache com o contador de execuções parado.
- [ ] Justifico o erro memorizado e o pedido sem `id` rejeitado.
- [ ] Aponto o teto do cache e digo o que o expira em produção.
- [ ] Distingo RPC (visão do cliente) de consenso (decisão do conjunto).

**Referências primárias:** `man 2 recv` (repetição é propriedade da rede, não defeito dela); a especificação de [Idempotency-Key do Stripe](https://docs.stripe.com/api/idempotent_requests) como exemplo industrial do mesmo `id`; [Raft](https://raft.github.io/raft.pdf) §8 para por que o RPC sozinho não é consenso.

*Atividades e avaliação desta aula serão publicadas ao fim do volume, junto com as demais — por decisão editorial, o conteúdo vem primeiro.*
