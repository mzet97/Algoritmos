---
layout: default
---

# Aula 23e — RPC: o pedido tem nome, a resposta tem memória

Você vai ao terminar, você deverá numerar cada pedido com `id`, servir repetição sem reexecutar (inclusive o erro), rejeitar pedido sem `id` — explicar por que retry sem `id` é duplicação com passos extras — e dizer exatamente onde o contador `execucoes` pararia se o cache fosse consultado depois de executar em vez de antes.

redes repetem. Timeout não diz se o servidor executou e a resposta se perdeu, ou se o pedido nunca chegou. Sem `id`, o cliente que tenta de novo aposta: pode estar pedindo pela primeira ou pela segunda vez sem saber. Com `id`, a aposta some: repetir é pedir a resposta gravada. E a aposta some para os dois desfechos — acerto e erro — porque o cliente repete o erro tanto quanto repete o acerto.

## 1. Contrato antes do código

- **Pedido:** `(id, método, argumento)`. Sem `id`, sem resposta — o servidor recusa na porta, sem olhar o método.
- **Resposta:** `(id, resultado, repetição?)`. `id` novo executa e grava; `id` repetido devolve o gravado com a marca `repetição`.
- **Erro também é resposta:** método inválido grava o erro sob o `id`. Reavaliar o erro a cada retry é reabrir uma decisão tomada — e uma decisão reaberta sob carga pode mudar de ideia.
- **Limite:** 16 pedidos gravados, `id` e método com teto de tamanho. Cache sem teto é vazamento com nome bonito: cresce com o número de clientes e nunca encolhe.
- **Fora do contrato:** serialização binária, prazos e cancelamento — o `id` sobrevive a todos eles, mas mora numa camada abaixo. O transporte continua sendo chamada de função (a rede mora na 23a); aqui o objeto é a semântica.

## 2. A ambiguidade que o timeout cria (e o id desfaz)

Desenhe a linha do tempo de um retry sem `id`. Em t0 o cliente envia; em t1 o servidor executa; em t2 a resposta se perde; em t3 o timeout dispara; em t4 o cliente reenvia "o mesmo" pedido. Para o servidor, t4 é indistinguível de um pedido novo — ele executa de novo. Se o método era "debitar 10", debitou 20. O cliente não errou o retry (era o certo a fazer) e o servidor não errou a execução (cada pedido parecia novo). O erro está na ausência de identidade: sem `id`, "o mesmo pedido" não existe no vocabulário do servidor.

Com `id`, t4 carrega "req-1" e o servidor consulta o cache antes de executar: resposta gravada, contador parado, cliente satisfeito. A consulta vem *antes* da execução — a ordem é o mecanismo inteiro. Invertê-la (executar, depois verificar se já tinha) executa duas vezes e grava por cima: o contador andaria e o teste o pegaria. Todo o RPC idempotente cabe nesta frase: cheque o nome antes de fazer o trabalho.

## 3. Por que o erro também se memoriza?

Porque o cliente repete o erro tanto quanto repete o acerto: o retry não distingue desfecho, só distingue resposta de silêncio. Se o servidor reexecutar o método inválido a cada repetição, cada retry custa uma decisão nova sobre um fato velho — e um método com efeito colateral parcial pode completar na segunda tentativa o que recusou na primeira. Imagine "reservar assento": a primeira tentativa falha por assento ocupado (erro gravável); sem memória do erro, a segunda tentativa reexecuta, o assento vagou no meio tempo, e o cliente recebe sucesso para um pedido que já tinha recebido erro. O mesmo `id`, dois desfechos: a identidade do pedido foi violada.

Erro gravado congela a decisão: o pedido morreu uma vez, e morto fica. O cenário 3 do exemplo prova com o contador parado em 2 — a repetição do erro não reexecuta, exatamente como a repetição do acerto. Decisão gravada vale para os dois desfechos, sem exceção, porque a alternativa (reavaliar) quebra a promessa do `id` na metade dos casos.

## 4. O teto do cache é parte do contrato

16 pedidos, `id` de 32 bytes, método de 16. Três tetos, três ameaças contidas: o número de entradas limita a memória total (16 × ~56 bytes, fração de KiB — gravável até em kernel); o teto do `id` impede chave gigante como vetor de DoS (`snprintf` com teto nunca estoura o destino); o teto do método impede despacho sobre string arbitrária. O `assert(n_cache < MAX_PEDIDOS)` no estouro é a versão didática da expiração: em produção, o teto vira expiração por tempo (TTL) ou confirmação pelo cliente (ack remove) — o cliente que já leu a resposta não precisa que o servidor a guarde para sempre.

Note a escolha deliberada de falhar alto no estouro em vez de despejar silenciosamente: despejo silencioso (LRU) transformaria retry legítimo em reexecução sem aviso — a idempotência quebraria quieta. Em produção com TTL, o retry após expiração reexecuta *declaradamente* (o contrato documenta a janela); aqui, o `assert` documenta que a janela ainda não existe.

## 5. Exemplo completo e executável

O programa abaixo é o [arquivo-fonte desta aula](exemplos/23e_rpc.c), reproduzido integralmente:

```c
/* Aula 23e — RPC sobre a base do consenso: id, repeticao e resposta.
 *
 * O transporte continua sendo chamada de funcao (a rede mora na 23a); aqui o
 * objeto e a semantica: cada pedido carrega id, o servidor responde uma vez
 * por id, e o cliente que repete recebe a mesma resposta sem reexecutar.
 * O commit (23d) decide O QUE vale; o RPC decide COMO o cliente ve.
 *
 * Compilar: gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/23e_rpc.c -o /tmp/23e
 * Executar: /tmp/23e
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define MAX_PEDIDOS 16

typedef struct {
    char id[32];
    char metodo[16];
    int argumento;
    int resposta;
    int respondido; /* 0 = novo, 1 = repeticao servida do cache */
} pedido_t;

typedef struct {
    char id[32];
    int resposta;
} cache_t;

static cache_t cache[MAX_PEDIDOS];
static int n_cache = 0;
static int execucoes = 0;

static int executar(const pedido_t *p)
{
    execucoes++;
    if (strcmp(p->metodo, "dobrar") == 0) {
        return p->argumento * 2;
    }
    if (strcmp(p->metodo, "somar1") == 0) {
        return p->argumento + 1;
    }
    return -1;
}

/* Servidor: id repetido devolve a resposta gravada; id novo executa e grava. */
static int servir(const pedido_t *pedido, pedido_t *resposta)
{
    if (strlen(pedido->id) == 0 || strlen(pedido->metodo) == 0) {
        return -1; /* pedido malformado: sem id, sem resposta */
    }
    for (int i = 0; i < n_cache; i++) {
        if (strcmp(cache[i].id, pedido->id) == 0) {
            *resposta = *pedido;
            resposta->resposta = cache[i].resposta;
            resposta->respondido = 1;
            return 0;
        }
    }
    assert(n_cache < MAX_PEDIDOS);
    snprintf(cache[n_cache].id, sizeof cache[n_cache].id, "%s", pedido->id);
    cache[n_cache].resposta = executar(pedido);
    n_cache++;
    *resposta = *pedido;
    resposta->resposta = cache[n_cache - 1].resposta;
    resposta->respondido = 0;
    return 0;
}

int main(void)
{
    pedido_t r;

    /* Chamada normal: executa uma vez. */
    assert(servir(&(pedido_t){ .id = "req-1", .metodo = "dobrar", .argumento = 21 }, &r) == 0);
    assert(r.resposta == 42 && r.respondido == 0);
    printf("rpc req-1 dobrar(21) = %d (executou)\n", r.resposta);

    /* Repeticao (retry do cliente, duplicata da rede): mesma resposta, sem
     * reexecutar — a prova esta no contador de execucoes. */
    assert(servir(&(pedido_t){ .id = "req-1", .metodo = "dobrar", .argumento = 21 }, &r) == 0);
    assert(r.resposta == 42 && r.respondido == 1);
    assert(execucoes == 1);
    printf("rpc req-1 repetido = %d (cache, execucoes=%d)\n", r.resposta, execucoes);

    /* Metodo desconhecido tambem e resposta gravada: o erro nao se reavalia. */
    assert(servir(&(pedido_t){ .id = "req-2", .metodo = "voar", .argumento = 0 }, &r) == 0);
    assert(r.resposta == -1 && r.respondido == 0);
    assert(servir(&(pedido_t){ .id = "req-2", .metodo = "voar", .argumento = 0 }, &r) == 0);
    assert(r.respondido == 1 && execucoes == 2);
    printf("rpc req-2 metodo invalido = erro gravado, repeticao servida do cache\n");

    /* Sem id nao ha resposta: o cliente aprende a numerar antes de chamar. */
    assert(servir(&(pedido_t){ .id = "", .metodo = "dobrar", .argumento = 1 }, &r) == -1);
    printf("rpc sem id: rejeitado (sem id, sem resposta)\n");

    printf("23e ok: idempotencia por id, retry seguro, erro tambem memorizado\n");
    return 0;
}
```

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

## 6. Leitura guiada da saída

A prova está no contador `execucoes`: a repetição de `req-1` não o move de 1 — retry seguro não é retry que "dá certo", é retry que não executa de novo. Note que a resposta da repetição carrega a marca `respondido=1`: o cliente distingue "executei agora" de "lembrei de antes", e essa distinção permite ao cliente decidir se confia (resposta fresca) ou se aceita (resposta lembrada) — sem a marca, o retry seria indistinguível da primeira chamada e o cliente nunca saberia se sua ação foi aplicada uma vez ou gravada antes.

O cenário do erro merece leitura lenta: `req-2` com método `"voar"` executa (contador vai a 2) e grava `-1`; a repetição devolve `-1` com a marca, sem executar. Se o método ganhasse efeito colateral amanhã (logar, debitar, enviar), a repetição continuaria sem efeito — a idempotência foi comprada no desenho, antes do método existir. E o cenário sem `id` fecha a porta: `servir` retorna `-1` sem tocar no cache nem executar — pedido sem nome não existe para o servidor, e a recusa na porta é mais barata que qualquer tratamento depois.

### O que a memória faz aqui?

Cache com teto (`MAX_PEDIDOS`, `assert` no estouro): em produção, o teto vira expiração por tempo ou confirmação — o cliente que já leu a resposta não precisa que o servidor a guarde para sempre. `id` copiado com `snprintf` e teto: `id` sem teto é o abuso da 32c de volta (chave gigante como DoS). Resposta copiada por struct (`*resposta = *pedido`): sem aliasing entre pedido e cache — o chamador pode reutilizar o buffer do pedido sem corromper o gravado.

## 7. Erros comuns e implicações defensivas

1. **Retry sem `id`:** cada tentativa é um pedido novo. Efeito executado duas vezes não é bug da rede — é pedido mal numerado. A rede repete por natureza; numerar é dever do cliente.
2. **Gravar só o acerto:** o erro reavaliado muda de ideia sob carga (o exemplo do assento que vaga). Decisão gravada vale para os dois desfechos — sem exceção, porque a exceção quebra a promessa na metade dos casos.
3. **Consultar o cache depois de executar:** a ordem é o mecanismo. Executar-antes-de-checar executa sempre e grava por cima: o contador andaria e o teste pegaria. Cheque o nome antes de fazer o trabalho.
4. **Cache sem teto:** crescimento proporcional ao número de clientes é negação de serviço adiada. Teto e expiração são parte do contrato, não otimização futura.
5. **Confundir RPC com consenso:** o RPC entrega a resposta a um cliente; o consenso decide o que vale para todos. Um não substitui o outro — o RPC desta aula roda *sobre* o commit da 23d. Servidor que responde rápido mas sem commit conta história, não verdade.
6. **`id` reutilizado para pedido diferente:** o servidor devolve a resposta do pedido antigo — corretamente, segundo o contrato. `id` é identidade do pedido, não do cliente: reutilizar é mentir sobre o que se pediu, e o servidor não tem como saber.
7. **`id` previsível em operação sensível:** `req-1, req-2, ...` permite a um atacante adivinhar `id` e colher respostas alheias do cache. Em produção com múltiplos clientes, `id` é aleatório (128 bits) e autenticado — o contador desta aula vira nonce com dono.

## 8. Ponte com as demais trilhas

Go cobra chave de idempotência antes de qualquer retry de escrita (a mesma porta, com HTTP no meio); Python dedup por `(autor, seq)` na reconciliação da 23b (o `id` composto quando há múltiplos autores); Rust traduz `id` em tipo que não se confunde (`Id` novo em vez de `String` solta); C++ memoriza o erro junto com o acerto na 23f (repetição servida do cache, inclusive o desfecho ruim). Em JavaScript, o `id` é o `Idempotency-Key` dos gateways de pagamento: o duplo clique no "pagar" é o retry desta aula com roupa de usuário — e a janela de 24h do gateway é o TTL que esta aula declara como futuro.

---


A execucao bem-sucedida sozinha nao aprova: a maior parcela esta em demonstrar que voce entendeu o porque.

### Para se aprofundar

1. Martin Kleppmann, *Designing Data-Intensive Applications* (O'Reilly, ISBN 978-1449373320), caps. 8-9; 2. *Raft paper* (Ongaro e Ousterhout, [raft.github.io](https://raft.github.io/)); 3. W. Richard Stevens et al., *UNIX Network Programming, vol. 1* (Addison-Wesley, 3. ed., ISBN 978-0131411555).

## 9. Ao final, você consegue

- [ ] Explico por que timeout não distingue "não executou" de "resposta perdida" — e como o `id` resolve (consulta antes da execução).
- [ ] Demonstro repetição servida do cache com o contador de execuções parado, e digo onde ele pararia se a ordem invertesse.
- [ ] Justifico o erro memorizado com o cenário do efeito colateral parcial, e o pedido sem `id` rejeitado na porta.
- [ ] Aponto o teto do cache e digo o que o expira em produção (TTL ou ack) — e por que despejo silencioso quebraria a promessa.
- [ ] Distingo RPC (visão do cliente) de consenso (decisão do conjunto), e `id` do pedido de `id` do cliente.
- [ ] Descrevo o ataque do `id` previsível e a correção (nonce aleatório autenticado).

**Referências primárias:** `man 2 recv` (repetição é propriedade da rede, não defeito dela); a especificação de [Idempotency-Key do Stripe](https://docs.stripe.com/api/idempotent_requests) como exemplo industrial do mesmo `id` (com janela de 24h = TTL documentado); [Raft](https://raft.github.io/raft.pdf) §8 para por que o RPC sozinho não é consenso (o líder filtra duplicatas *após* o commit decidir).

## 10. Exercícios obrigatórios

1. (Previsão) Reenvie `req-1` com argumento diferente (`dobrar(100)`): preveja — volta 42 ou 200? Rode e explique por que o cache é indexado por `id`, não pelo conteúdo — e o perigo de um cliente que recicla `id`.
2. (Diagnóstico) Remova a rejeição de `id` vazio (apague a primeira guarda de `servir`): preveja quantas execuções dois pedidos sem `id` causam. Rode e explique por que "sem `id`, sem resposta" é a trava contra colisão.
3. (Projeto) Envie 17 `id`s distintos e observe o `assert(n_cache < MAX_PEDIDOS)` abortar. Substitua o abort por erro explícito (retorne `-2` = cache cheio, sem gravar) e teste: o 17º pedido falha, os 16 anteriores continuam servindo do cache.
4. (Previsão) O `id` chega da rede num buffer maior; o servidor o copia para `char[32]` com truncamento. Preveja: dois `id`s longos que só diferem após o 31º caractere colidem? Prove com um programa mínimo e corrija `servir` rejeitando `id` com 31 ou mais caracteres.

Use [pistas graduais](exercicios/23e_pistas.md) somente após uma tentativa. Consulte as [soluções comentadas](exercicios/23e_solucoes.md) depois de registrar sua própria explicação.
