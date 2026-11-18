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
