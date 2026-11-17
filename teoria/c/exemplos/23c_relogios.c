/* Aula 23c - relogios logicos, causalidade e particao bilateral.
 * Simulacao deterministica em um processo (sem sockets): o objeto de
 * estudo e a ORDEM dos eventos, nao o transporte. Transporte e a 23a. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOS 2
#define MAX_EV 64

typedef struct {
    int lamport;
    int vetor[NOS];
    int autor;          /* no que gerou o evento */
    int seq;            /* sequencia local do autor (desempate estavel) */
    char chave[16];
    int valor;
} Evento;

typedef struct {
    int lamport;
    int vetor[NOS];
    Evento log[MAX_EV];
    int nlog;
    /* estado materializado: ultimo evento aplicado por chave, na ordem
       total (lamport, autor) - a regra de reconciliacao declarada */
    char chaves[MAX_EV][16];
    int valores[MAX_EV];
    int nchaves;
} NoSim;

static void aplicar(NoSim *no, const Evento *e) {
    for (int i = 0; i < no->nchaves; i++) {
        if (strcmp(no->chaves[i], e->chave) == 0) {
            no->valores[i] = e->valor;
            return;
        }
    }
    snprintf(no->chaves[no->nchaves], 16, "%s", e->chave);
    no->valores[no->nchaves] = e->valor;
    no->nchaves++;
}

static int ler(const NoSim *no, const char *chave, int *saida) {
    for (int i = 0; i < no->nchaves; i++)
        if (strcmp(no->chaves[i], chave) == 0) { *saida = no->valores[i]; return 1; }
    return 0;
}

/* Escrita local: avanca Lamport e o proprio componente do vetor. */
static Evento escrever(NoSim *no, int autor, int seq, const char *chave, int valor) {
    no->lamport++;
    no->vetor[autor]++;
    Evento e;
    memset(&e, 0, sizeof e);
    e.lamport = no->lamport;
    memcpy(e.vetor, no->vetor, sizeof e.vetor);
    e.autor = autor;
    e.seq = seq;
    snprintf(e.chave, sizeof e.chave, "%s", chave);
    e.valor = valor;
    assert(no->nlog < MAX_EV);
    no->log[no->nlog++] = e;
    aplicar(no, &e);
    return e;
}

/* Receber: Lamport = max(local, recebido) + 1; vetor = max componente a
   componente. Devolve 0 se o evento ja era conhecido (duplicata). */
static int receber(NoSim *no, const Evento *e) {
    for (int i = 0; i < no->nlog; i++)
        if (no->log[i].autor == e->autor && no->log[i].seq == e->seq) return 0;
    no->lamport = (no->lamport > e->lamport ? no->lamport : e->lamport) + 1;
    for (int i = 0; i < NOS; i++)
        if (e->vetor[i] > no->vetor[i]) no->vetor[i] = e->vetor[i];
    assert(no->nlog < MAX_EV);
    no->log[no->nlog++] = *e;
    return 1;
}

/* e1 aconteceu-antes de e2? Pelo vetor: e1 <= e2 componente a componente
   e estritamente menor em algum. Concorrentes: nenhum dos dois sentidos. */
static int antes(const Evento *a, const Evento *b) {
    int menor = 0;
    for (int i = 0; i < NOS; i++) {
        if (a->vetor[i] > b->vetor[i]) return 0;
        if (a->vetor[i] < b->vetor[i]) menor = 1;
    }
    return menor;
}
static int concorrentes(const Evento *a, const Evento *b) {
    return !antes(a, b) && !antes(b, a) && !(a->autor == b->autor && a->seq == b->seq);
}

static int cmp_evento(const void *pa, const void *pb) {
    const Evento *a = pa, *b = pb;
    if (a->lamport != b->lamport) return a->lamport - b->lamport;
    if (a->autor != b->autor) return a->autor - b->autor;
    return a->seq - b->seq;
}

/* Anti-entropia: troca os logs inteiros e reaplica na ordem total
   (lamport, autor, seq). Quem ja conhecia o evento ignora a copia. */
static void sincronizar(NoSim *a, NoSim *b) {
    Evento copia_a[MAX_EV], copia_b[MAX_EV];
    int na = a->nlog, nb = b->nlog;
    memcpy(copia_a, a->log, (size_t)na * sizeof(Evento));
    memcpy(copia_b, b->log, (size_t)nb * sizeof(Evento));
    for (int i = 0; i < nb; i++) (void)receber(a, &copia_b[i]);
    for (int i = 0; i < na; i++) (void)receber(b, &copia_a[i]);
    /* Reconstroi o estado dos dois na ordem total declarada. */
    NoSim *ambos[NOS] = {a, b};
    for (int k = 0; k < NOS; k++) {
        NoSim *no = ambos[k];
        qsort(no->log, (size_t)no->nlog, sizeof(Evento), cmp_evento);
        /* Deduplica apos ordenar (reenvios podem ter entrado antes). */
        int m = 0;
        for (int i = 0; i < no->nlog; i++) {
            int dup = 0;
            for (int j = 0; j < m; j++)
                if (no->log[j].autor == no->log[i].autor && no->log[j].seq == no->log[i].seq) dup = 1;
            if (!dup) no->log[m++] = no->log[i];
        }
        no->nlog = m;
        no->nchaves = 0;
        for (int i = 0; i < no->nlog; i++) aplicar(no, &no->log[i]);
    }
}

int main(void) {
    NoSim A, B;
    memset(&A, 0, sizeof A);
    memset(&B, 0, sizeof B);
    int v;

    /* Fase 1: causalidade em cadeia A -> B -> A. */
    Evento e1 = escrever(&A, 0, 1, "x", 10);
    assert(receber(&B, &e1) == 1);
    assert(receber(&B, &e1) == 0); /* duplicata reconhecida */
    Evento e2 = escrever(&B, 1, 1, "y", 20);
    assert(antes(&e1, &e2));       /* B so escreveu depois de ver e1 */
    assert(!concorrentes(&e1, &e2));
    printf("lamport: e1=%d e2=%d (causalidade preservada)\n", e1.lamport, e2.lamport);

    /* Fase 2: particao. Cada no escreve na MESMA chave sem ver o outro. */
    NoSim A2, B2;
    memset(&A2, 0, sizeof A2);
    memset(&B2, 0, sizeof B2);
    Evento p1 = escrever(&A2, 0, 1, "k", 111);
    Evento p2 = escrever(&B2, 1, 1, "k", 222);
    assert(concorrentes(&p1, &p2)); /* nenhum aconteceu-antes: particao real */
    assert(ler(&A2, "k", &v) && v == 111);
    assert(ler(&B2, "k", &v) && v == 222);
    printf("particao: A ve k=111, B ve k=222, eventos concorrentes detectados\n");

    /* Fase 3: anti-entropia + ordem total (lamport, autor, seq). Os
       eventos sao reaplicados em ordem crescente, logo o ULTIMO da
       ordem total vence: empate de Lamport (1,1) decide pelo autor,
       e a escrita perdedora e descartada - perda DECLARADA. */
    sincronizar(&A2, &B2);
    assert(ler(&A2, "k", &v) && v == 222);
    assert(ler(&B2, "k", &v) && v == 222);
    printf("reconciliacao: ambos convergem para k=222; k=111 descartado pela ordem total (lamport,autor)\n");

    /* Fase 4: apos sincronizar, nova escrita causal supera as antigas. */
    Evento e3 = escrever(&B2, 1, 2, "k", 333);
    assert(antes(&p1, &e3) && antes(&p2, &e3));
    sincronizar(&A2, &B2);
    assert(ler(&A2, "k", &v) && v == 333);
    assert(ler(&B2, "k", &v) && v == 333);
    printf("pos-sincronizacao: escrita causal k=333 vence nos dois nos\n");

    printf("23c ok: Lamport ordena, vetor detecta concorrencia, particao reconciliada com perda declarada\n");
    return EXIT_SUCCESS;
}
