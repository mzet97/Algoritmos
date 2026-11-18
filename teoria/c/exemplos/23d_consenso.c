/* Aula 23d — consenso didactico: eleicao, quorum e commit por maioria.
 *
 * Simulacao deterministica em um processo: 3 nos, sem rede e sem tempo.
 * O "transporte" e chamada de funcao; o objeto e a regra de seguranca:
 * um comando so e confirmado quando esta na maioria, e a minoris nao confirma.
 *
 * Compilar: gcc -std=c11 -Wall -Wextra -Wpedantic -Werror teoria/c/exemplos/23d_consenso.c -o /tmp/23d
 * Executar: /tmp/23d
 */
#include <assert.h>
#include <stdio.h>

#define NOS 3
#define MAIORIA 2
#define LOG_MAX 16

typedef enum { SEGUIDOR = 0, CANDIDATO, LIDER } papel_t;

typedef struct {
    int termo;
    int comando; /* carga util didactica: um inteiro por entrada */
} entrada_t;

typedef struct {
    int id;
    int termo;
    papel_t papel;
    int votou_em; /* -1 = ninguem neste termo */
    entrada_t log[LOG_MAX];
    int tamanho;
    int commit;
} no_t;

static void no_init(no_t *no, int id)
{
    no->id = id;
    no->termo = 0;
    no->papel = SEGUIDOR;
    no->votou_em = -1;
    no->tamanho = 0;
    no->commit = -1;
}

/* O candidato esta pelo menos tao atualizado quanto o eleitor? */
static int atualizado(no_t *cand, no_t *eleitor)
{
    if (cand->termo != eleitor->termo) {
        return cand->termo >= eleitor->termo;
    }
    return cand->tamanho >= eleitor->tamanho;
}

/* Uma eleicao num subconjunto de nos (particao). Devolve o id do lider ou -1. */
static int eleicao(no_t nos[NOS], const int *membros, int n_membros)
{
    int candidato = membros[0];
    no_t *c = &nos[candidato];

    c->termo += 1;
    c->papel = CANDIDATO;
    c->votou_em = candidato;

    for (int i = 0; i < n_membros; i++) {
        nos[membros[i]].termo = c->termo;
    }

    int votos = 1;
    for (int i = 1; i < n_membros; i++) {
        no_t *eleitor = &nos[membros[i]];
        if (eleitor->votou_em == -1 && atualizado(c, eleitor)) {
            eleitor->votou_em = candidato;
            votos++;
        }
    }
    if (votos >= MAIORIA) {
        c->papel = LIDER;
        /* Termo novo invalida votos antigos em quem nao votou. */
        for (int i = 0; i < n_membros; i++) {
            if (nos[membros[i]].votou_em == -1) {
                nos[membros[i]].votou_em = -2; /* absteve-se neste termo */
            }
        }
        return candidato;
    }
    c->papel = SEGUIDOR;
    return -1;
}

/* O lider replica a entrada para os membros alcancaveis; confirma por maioria. */
static int replicar(no_t nos[NOS], int lider, const int *membros, int n_membros,
                    int comando)
{
    no_t *l = &nos[lider];
    assert(l->papel == LIDER);
    assert(l->tamanho < LOG_MAX);

    l->log[l->tamanho].termo = l->termo;
    l->log[l->tamanho].comando = comando;
    l->tamanho++;

    int acks = 1;
    for (int i = 0; i < n_membros; i++) {
        int id = membros[i];
        if (id == lider) {
            continue;
        }
        no_t *s = &nos[id];
        if (s->termo != l->termo) {
            continue; /* termo antigo: nao segue este lider */
        }
        assert(s->tamanho < LOG_MAX);
        s->log[s->tamanho] = l->log[l->tamanho - 1];
        s->tamanho++;
        acks++;
    }
    if (acks >= MAIORIA) {
        l->commit = l->tamanho - 1;
        for (int i = 0; i < n_membros; i++) {
            if (nos[membros[i]].termo == l->termo) {
                nos[membros[i]].commit = l->commit;
            }
        }
        return 1;
    }
    return 0;
}

static void imprime_estado(const char *fase, no_t nos[NOS])
{
    printf("%s:", fase);
    for (int i = 0; i < NOS; i++) {
        printf(" N%d(t=%d,%s,log=%d,commit=%d)", nos[i].id, nos[i].termo,
               nos[i].papel == LIDER ? "lider" : "seg",
               nos[i].tamanho, nos[i].commit);
    }
    printf("\n");
}

int main(void)
{
    no_t nos[NOS];
    for (int i = 0; i < NOS; i++) {
        no_init(&nos[i], i);
    }

    /* Fase 1: eleicao com os 3 nos e um comando confirmado por maioria. */
    const int todos[NOS] = { 0, 1, 2 };
    int lider = eleicao(nos, todos, NOS);
    assert(lider == 0);
    assert(replicar(nos, lider, todos, NOS, 111) == 1);
    imprime_estado("fase1 lider+commit", nos);

    /* Fase 2: particao {0,1} | {2}. A minoria de 1 nao elege nem confirma. */
    const int maj[] = { 0, 1 };
    const int minoria[] = { 2 };
    int lider_min = eleicao(nos, minoria, 1);
    printf("fase2 minoria tenta eleger: %s (votos=1, quorum=2)\n",
           lider_min == -1 ? "sem lider" : "LIDER (erro)");
    assert(lider_min == -1);
    /* A escrita solitaria da minoria nao confirma: sem maioria, sem commit. */
    nos[2].log[nos[2].tamanho].termo = nos[2].termo;
    nos[2].log[nos[2].tamanho].comando = 222;
    nos[2].tamanho++;
    printf("fase2 escrita da minoria: log=222 mas commit=%d (nao confirmado)\n",
           nos[2].commit);
    assert(nos[2].commit == 0);

    /* Fase 3: a maioria restante ainda confirma sem o no isolado. */
    assert(replicar(nos, 0, maj, 2, 333) == 1);
    imprime_estado("fase3 maioria confirma 333", nos);

    /* Fase 4: cura. Novo termo, eleicao com todos, o lider sobrescreve a
     * entrada nao confirmada da ex-minoria com o log confirmado. */
    for (int i = 0; i < NOS; i++) {
        nos[i].papel = SEGUIDOR;
        nos[i].votou_em = -1;
    }
    lider = eleicao(nos, todos, NOS);
    assert(lider != -1);
    /* Sincroniza: o log do lider (confirmado) vence; a entrada 222 some. */
    for (int i = 0; i < NOS; i++) {
        if (i != lider) {
            for (int j = 0; j < nos[lider].tamanho; j++) {
                nos[i].log[j] = nos[lider].log[j];
            }
            nos[i].tamanho = nos[lider].tamanho;
            nos[i].commit = nos[lider].commit;
        }
    }
    imprime_estado("fase4 pos-cura", nos);
    for (int i = 0; i < NOS; i++) {
        assert(nos[i].tamanho == 2);
        assert(nos[i].log[0].comando == 111);
        assert(nos[i].log[1].comando == 333);
        assert(nos[i].commit == 1);
    }
    printf("23d ok: eleicao por maioria, commit por maioria, minoria nao confirma\n");
    return 0;
}
