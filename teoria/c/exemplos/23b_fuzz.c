/* Aula 23b - fuzzing deterministico de um parser de protocolo.
 * O parser recebe (ponteiro, comprimento): NUNCA presume '\0'.
 * O fuzzer muta linhas validas com LCG de semente fixa; qualquer crash,
 * leitura fora do limite ou resultado indefinido e falha (com ASan, aborta). */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINHA_MAX 512
#define ID_MAX 31
#define CHAVE_MAX 63
#define VALOR_MAX 127

typedef enum { PARSE_PUT, PARSE_GET, PARSE_ERRO } ParseKind;

typedef struct {
    ParseKind kind;
    char id[ID_MAX + 1];
    char chave[CHAVE_MAX + 1];
    char valor[VALOR_MAX + 1];
    int encaminhado;
} Pedido;

static int eh_token_char(unsigned char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') || c == '.' || c == '_' || c == '-';
}

/* Copia token [ini,fim) se couber e for valido; 1=ok. */
static int copiar_token(char *dest, size_t dest_cap, const char *buf,
                        size_t ini, size_t fim) {
    size_t n = fim - ini;
    if (n == 0 || n >= dest_cap) return 0;
    for (size_t i = ini; i < fim; i++)
        if (!eh_token_char((unsigned char)buf[i])) return 0;
    memcpy(dest, buf + ini, n);
    dest[n] = '\0';
    return 1;
}

/* Analisa uma linha SEM '\n'. Resultado sempre definido. */
static ParseKind analisar(const char *buf, size_t len, Pedido *out) {
    memset(out, 0, sizeof *out);
    out->kind = PARSE_ERRO;
    if (len == 0 || len > LINHA_MAX) return PARSE_ERRO;
    /* Fatia em ate 6 tokens separados por espaco, usando so [0,len). */
    size_t ini[6], fim[6];
    int ntok = 0;
    size_t i = 0;
    while (i < len) {
        while (i < len && (buf[i] == ' ' || buf[i] == '\t' || buf[i] == '\r')) i++;
        if (i >= len) break;
        if (ntok == 6) return PARSE_ERRO;
        ini[ntok] = i;
        while (i < len && buf[i] != ' ' && buf[i] != '\t' && buf[i] != '\r') i++;
        fim[ntok] = i;
        ntok++;
    }
    if (ntok == 0) return PARSE_ERRO;
    size_t op_n = fim[0] - ini[0];
    if (op_n == 3 && memcmp(buf + ini[0], "GET", 3) == 0) {
        if (ntok != 2) return PARSE_ERRO;
        if (!copiar_token(out->chave, sizeof out->chave, buf, ini[1], fim[1]))
            return PARSE_ERRO;
        out->kind = PARSE_GET;
        return PARSE_GET;
    }
    if (op_n == 3 && memcmp(buf + ini[0], "PUT", 3) == 0) {
        if (ntok != 4 && ntok != 5) return PARSE_ERRO;
        if (ntok == 5) {
            if (fim[4] - ini[4] != 1 || buf[ini[4]] != 'F') return PARSE_ERRO;
            out->encaminhado = 1;
        }
        if (!copiar_token(out->id, sizeof out->id, buf, ini[1], fim[1])) return PARSE_ERRO;
        if (!copiar_token(out->chave, sizeof out->chave, buf, ini[2], fim[2])) return PARSE_ERRO;
        if (!copiar_token(out->valor, sizeof out->valor, buf, ini[3], fim[3])) return PARSE_ERRO;
        out->kind = PARSE_PUT;
        return PARSE_PUT;
    }
    return PARSE_ERRO;
}

static uint32_t lcg_estado = 99u;
static uint32_t lcg_proximo(void) {
    lcg_estado = lcg_estado * 1664525u + 1013904223u;
    return lcg_estado >> 8;
}

static const char *bases[] = {
    "PUT id1 k1 v1", "PUT id-2 chave.x valor_9 F", "GET k1",
    "PUT a b c", "GET chave-longa_0123456789",
};
#define NBASES (sizeof bases / sizeof *bases)

int main(void) {
    Pedido p;
    /* Casos dirigidos: o contrato antes do caos. */
    assert(analisar("PUT id1 k1 v1", 13, &p) == PARSE_PUT && strcmp(p.valor, "v1") == 0);
    assert(analisar("PUT id1 k1 v1 F", 15, &p) == PARSE_PUT && p.encaminhado == 1);
    assert(analisar("GET k1", 6, &p) == PARSE_GET && strcmp(p.chave, "k1") == 0);
    assert(analisar("PUT id1 k1", 10, &p) == PARSE_ERRO);          /* falta token */
    assert(analisar("PUT id1 k1 v1 X", 15, &p) == PARSE_ERRO);    /* 5o token invalido */
    assert(analisar("DROP k1", 7, &p) == PARSE_ERRO);             /* op desconhecida */
    assert(analisar("", 0, &p) == PARSE_ERRO);                    /* vazia */
    assert(analisar("PUT id$ k1 v1", 13, &p) == PARSE_ERRO);      /* fora do alfabeto */
    {
        char longa[600];
        memset(longa, 'a', sizeof longa);
        memcpy(longa, "GET ", 4);
        assert(analisar(longa, sizeof longa, &p) == PARSE_ERRO);  /* acima do teto */
    }
    /* Sem '\0': buffer cru de 13 bytes, parser trabalha so com len. */
    {
        char cru[13];
        memcpy(cru, "PUT id1 k1 v1", 13);
        assert(analisar(cru, 13, &p) == PARSE_PUT);
        assert(analisar(cru, 5, &p) == PARSE_ERRO); /* truncado no meio */
    }

    /* Fuzz deterministico: mutacoes de trocar, apagar, inserir e truncar. */
    size_t ok_put = 0, ok_get = 0, erros = 0;
    char mut[LINHA_MAX + 64];
    for (int iter = 0; iter < 200000; iter++) {
        const char *base = bases[lcg_proximo() % NBASES];
        size_t n = strlen(base);
        memcpy(mut, base, n);
        int mutacoes = 1 + (int)(lcg_proximo() % 3u);
        for (int m = 0; m < mutacoes && n > 0; m++) {
            uint32_t sorte = lcg_proximo();
            size_t pos = sorte % (uint32_t)n;
            switch ((sorte >> 8) % 4u) {
                case 0: mut[pos] = (char)(lcg_proximo() % 128u); break;      /* troca byte */
                case 1: memmove(mut + pos, mut + pos + 1, n - pos - 1); n--; break; /* apaga */
                case 2:                                                      /* insere */
                    if (n + 1 < sizeof mut) {
                        memmove(mut + pos + 1, mut + pos, n - pos);
                        mut[pos] = (char)(' ' + (lcg_proximo() % 90u));
                        n++;
                    }
                    break;
                default: n = pos; break;                                     /* trunca */
            }
        }
        Pedido q;
        ParseKind r = analisar(mut, n, &q);
        if (r == PARSE_PUT) {
            ok_put++;
            /* Invariante pos-parse: campos dentro dos tetos e tokens validos. */
            assert(strlen(q.id) <= ID_MAX && strlen(q.chave) <= CHAVE_MAX &&
                   strlen(q.valor) <= VALOR_MAX);
        } else if (r == PARSE_GET) {
            ok_get++;
            assert(strlen(q.chave) <= CHAVE_MAX);
        } else {
            erros++;
        }
    }
    printf("fuzz: 200000 iteracoes put=%zu get=%zu erro=%zu (semente 99)\n",
           ok_put, ok_get, erros);
    assert(ok_put > 0 && ok_get > 0 && erros > 0); /* as tres classes aparecem */
    printf("23b ok: parser definido sob mutacao, tetos e truncamento\n");
    return EXIT_SUCCESS;
}
