---
layout: default
---

# Aula 32: solucoes comentadas

Compare apos tentar os [exercicios](../32.md#18-exercicios-obrigatorios).

1–5. Loja de configuracao completa: copias proprias, remocao, carga de arquivo, visita por callback e testes embutidos:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ENTRADAS 64
#define MAX_CHAVE 32
#define MAX_VALOR 64

struct Loja {
    char chaves[MAX_ENTRADAS][MAX_CHAVE];
    char valores[MAX_ENTRADAS][MAX_VALOR];
    size_t quantidade;
};

static long loja_busca(const struct Loja *loja, const char *chave)
{
    for (size_t i = 0; i < loja->quantidade; i++) {
        if (strcmp(loja->chaves[i], chave) == 0) {
            return (long)i;
        }
    }
    return -1;
}

static int copia_cabe(char *destino, size_t cap, const char *origem)
{
    size_t n = strlen(origem);
    if (n >= cap) {
        return -1;
    }
    memcpy(destino, origem, n + 1);
    return 0;
}

/* 0 = ok; 1 = chave/valor nao cabem; 2 = loja cheia */
static int loja_define(struct Loja *loja, const char *chave, const char *valor)
{
    long existente = loja_busca(loja, chave);
    if (existente >= 0) {
        return copia_cabe(loja->valores[existente], MAX_VALOR, valor) == 0
            ? 0 : 1;
    }
    if (loja->quantidade == MAX_ENTRADAS) {
        return 2;
    }
    if (copia_cabe(loja->chaves[loja->quantidade], MAX_CHAVE, chave) != 0 ||
        copia_cabe(loja->valores[loja->quantidade], MAX_VALOR, valor) != 0) {
        return 1;
    }
    loja->quantidade++;
    return 0;
}

static const char *loja_obtem(const struct Loja *loja, const char *chave)
{
    long i = loja_busca(loja, chave);
    return i >= 0 ? loja->valores[i] : NULL;
}

/* 0 = removeu; 1 = chave ausente (politica: nao e erro fatal, e resposta) */
static int loja_remove(struct Loja *loja, const char *chave)
{
    long i = loja_busca(loja, chave);
    if (i < 0) {
        return 1;
    }
    size_t pos = (size_t)i;
    for (size_t j = pos; j + 1 < loja->quantidade; j++) {
        memcpy(loja->chaves[j], loja->chaves[j + 1], MAX_CHAVE);
        memcpy(loja->valores[j], loja->valores[j + 1], MAX_VALOR);
    }
    loja->quantidade--;
    return 0;
}

typedef void (*Visita)(void *ctx, const char *chave, const char *valor);

static void loja_para_cada(const struct Loja *loja, Visita visita, void *ctx)
{
    for (size_t i = 0; i < loja->quantidade; i++) {
        visita(ctx, loja->chaves[i], loja->valores[i]);
    }
}

/* Carrega "chave=valor" por linha; devolve quantas linhas ruins ignorou. */
static int loja_carrega(struct Loja *loja, const char *caminho, int *ignoradas)
{
    FILE *f = fopen(caminho, "r");
    if (f == NULL) {
        return -1;
    }
    char linha[128];
    *ignoradas = 0;
    while (fgets(linha, sizeof linha, f) != NULL) {
        linha[strcspn(linha, "\n")] = '\0';
        char *igual = strchr(linha, '=');
        if (igual == NULL) {
            (*ignoradas)++;
            continue;
        }
        *igual = '\0';
        if (loja_define(loja, linha, igual + 1) != 0) {
            (*ignoradas)++;
        }
    }
    fclose(f);
    return 0;
}

static void imprime(void *ctx, const char *chave, const char *valor)
{
    (void)ctx;
    printf("%s=%s\n", chave, valor);
}

int main(void)
{
    struct Loja loja = {{{0}}, {{0}}, 0};
    int falhas = 0;

    loja_define(&loja, "host", "localhost");
    loja_define(&loja, "porta", "8080");
    loja_define(&loja, "porta", "9090"); /* atualizacao */
    if (loja.quantidade != 2) falhas++;
    if (strcmp(loja_obtem(&loja, "porta"), "9090") != 0) falhas++;
    if (loja_obtem(&loja, "ausente") != NULL) falhas++;
    if (loja_remove(&loja, "host") != 0) falhas++;
    if (loja_remove(&loja, "host") != 1) falhas++; /* ausente: politica */
    if (loja.quantidade != 1) falhas++;

    /* Carga de arquivo: duas linhas boas e uma ruim (ignorada). */
    FILE *f = fopen("config_teste.txt", "w");
    if (f == NULL) {
        return 1;
    }
    fputs("modo=rapido\nlinha-sem-igual\nlimite=10\n", f);
    fclose(f);
    struct Loja do_arquivo = {{{0}}, {{0}}, 0};
    int ignoradas = 0;
    if (loja_carrega(&do_arquivo, "config_teste.txt", &ignoradas) != 0) falhas++;
    if (do_arquivo.quantidade != 2 || ignoradas != 1) falhas++;
    if (strcmp(loja_obtem(&do_arquivo, "limite"), "10") != 0) falhas++;
    remove("config_teste.txt");

    loja_para_cada(&loja, imprime, NULL);
    printf("testes: %s\n", falhas == 0 ? "ok" : "FALHOU");
    return falhas == 0 ? 0 : 1;
}
```

5. A loja **copia** chave e valor para buffers proprios na entrada: o chamador pode reutilizar ou liberar o buffer dele no instante seguinte, e a loja continua valida. Guardar o ponteiro do chamador criaria um contrato fragil — a loja passaria a depender do tempo de vida de memoria que ela nao controla, e cada cliente precisaria conhecer essa regra escondida. O preco sao os tetos `MAX_CHAVE`/`MAX_VALOR` declarados e o erro explicito quando nao cabe, em vez de truncamento silencioso.

6. Busca linear → hash sem quebrar a API: a API publica (`define`, `obtem`, `remove`, `para_cada` e seus contratos de erro) nao menciona o array; internamente, o array vira uma tabela com baldes e o `loja_busca` passa a calcular o hash da chave. Quem usa a loja nao recompila nem muda comportamento observavel — so a complexidade interna muda. E exatamente para comprar essa liberdade que o tipo foi mantido opaco no uso, como nas [aulas 18](18-solucoes.md) e 19.

**Autoverificacao:** `testes: ok` cobrindo atualizacao sem duplicar, chave ausente, remocao repetida com a politica declarada e listagem via callback sem acesso ao array.
