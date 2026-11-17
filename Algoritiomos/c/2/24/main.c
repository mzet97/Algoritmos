#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int col;
    int E;
    struct No * Prox;
} No;

typedef struct MatrizEsparsa {
    int N;
    No ** linhas;
} MatrizEsparsa;

/* Compute M^2 (M squared) for sparse matrix.
   Result[i][j] = sum over k of M[i][k] * M[k][j] */
MatrizEsparsa AoQuadrado(MatrizEsparsa M) {
    MatrizEsparsa R;
    R.N = M.N;
    R.linhas = (No **) malloc(sizeof(No *) * R.N);
    for (int i = 0; i < R.N; i++) {
        R.linhas[i] = NULL;
    }

    /* Transpose M for efficient column access */
    /* Build column lists: for each column j, list of (row, value) */
    No ** colunas = (No **) calloc(M.N, sizeof(No *));
    No ** colunas_tail = (No **) calloc(M.N, sizeof(No *));
    for (int i = 0; i < M.N; i++) {
        No * p = M.linhas[i];
        while (p != NULL) {
            No * novo = (No *) malloc(sizeof(No));
            novo->col = i; /* original row becomes "col" in transpose */
            novo->E = p->E;
            novo->Prox = NULL;
            if (colunas_tail[p->col] == NULL) {
                colunas[p->col] = novo;
            } else {
                colunas_tail[p->col]->Prox = novo;
            }
            colunas_tail[p->col] = novo;
            p = p->Prox;
        }
    }

    /* Compute result */
    No ** tails = (No **) calloc(R.N, sizeof(No *));
    for (int i = 0; i < M.N; i++) {
        No * pi = M.linhas[i];
        while (pi != NULL) {
            int k = pi->col;
            int mik = pi->E;
            /* Multiply with row k of M (column k of transpose = row k of M stored by col) */
            /* Actually we need M[k][j] for all j where M[k][j] != 0 */
            /* M[k][j] non-zero means column j of M has entry from row k */
            /* Wait: colunas[k] has entries (original_row, value) = (k was stored as col in transpose) */
            /* colunas[k] = entries from column k of M, each with original row as "col" */
            /* We need: for row i, for column j: sum_k M[i][k]*M[k][j] */
            /* pi gives us M[i][k]. Now we need all M[k][j] != 0. */
            /* M[k][j] is in M.linhas[k], iterate that */
            No * pk = M.linhas[k];
            while (pk != NULL) {
                int j = pk->col;
                int mkj = pk->E;
                /* Accumulate mik * mkj into R[i][j] */
                /* Since we iterate in order, we can just collect and sort later */
                /* For simplicity, we'll build unsorted and sort later */
                /* Actually the Escreve function doesn't sort, so we need to be careful */
                /* Let's just accumulate values by (i,j) pairs */
                No * novo = (No *) malloc(sizeof(No));
                novo->col = j;
                novo->E = mik * mkj;
                novo->Prox = NULL;
                if (tails[i] == NULL) {
                    R.linhas[i] = novo;
                } else {
                    tails[i]->Prox = novo;
                }
                tails[i] = novo;
                pk = pk->Prox;
            }
            pi = pi->Prox;
        }
    }

    /* Now combine same-column entries in each row */
    for (int i = 0; i < R.N; i++) {
        if (R.linhas[i] == NULL) continue;
        /* Count entries */
        int count = 0;
        No * p = R.linhas[i];
        while (p != NULL) { count++; p = p->Prox; }
        /* Sort by column using simple insertion sort */
        int * cols = (int *) malloc(sizeof(int) * count);
        int * vals = (int *) malloc(sizeof(int) * count);
        p = R.linhas[i];
        int idx = 0;
        while (p != NULL) {
            cols[idx] = p->col;
            vals[idx] = p->E;
            idx++;
            No * temp = p;
            p = p->Prox;
            free(temp);
        }
        /* Sort by column */
        for (int a = 0; a < count; a++) {
            for (int b = a+1; b < count; b++) {
                if (cols[b] < cols[a]) {
                    int tc = cols[a]; cols[a] = cols[b]; cols[b] = tc;
                    int tv = vals[a]; vals[a] = vals[b]; vals[b] = tv;
                }
            }
        }
        /* Merge same columns */
        No * head = NULL;
        No * tail = NULL;
        int a = 0;
        while (a < count) {
            int sum = vals[a];
            int c = cols[a];
            int b = a + 1;
            while (b < count && cols[b] == c) {
                sum += vals[b];
                b++;
            }
            No * novo = (No *) malloc(sizeof(No));
            novo->col = c;
            novo->E = sum;
            novo->Prox = NULL;
            if (head == NULL) head = novo;
            else tail->Prox = novo;
            tail = novo;
            a = b;
        }
        R.linhas[i] = head;
        free(cols); free(vals);
    }

    /* Free transpose data */
    for (int i = 0; i < M.N; i++) {
        No * p = colunas[i];
        while (p != NULL) {
            No * temp = p;
            p = p->Prox;
            free(temp);
        }
    }
    free(colunas); free(colunas_tail); free(tails);

    return R;
}

void InsereEnc(No ** L, int col, int e, No ** u) {
    No * novo = (No *) malloc(sizeof(No));
    novo->col = col; novo->E = e; novo->Prox = NULL;
    if (*u == NULL) {
        *L = novo;
    } else {
        (*u)->Prox = novo;
    }
    *u = novo;
}

void Escreve(MatrizEsparsa M) {
    No * p;
    printf("[");
    for (int i=0; i<M.N; i++) {
        printf("[");
        p = M.linhas[i];
        while (p != NULL) { printf("c=%d;e=%d ", p->col, p->E); p = p->Prox; };
        printf("]");
    }
    printf("]\n");
}

void DestroiLista(No * L) {
    No * p, * pr; p = L;
    while (p != NULL) {
        pr = p; p = p->Prox;
        free(pr);
    }
}

void DestroiMatriz(MatrizEsparsa M) {
    for (int i=0; i<M.N; i++) {
        DestroiLista(M.linhas[i]);
    }
    free(M.linhas);
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int n; No * L; No * u; int l; int c; int e; MatrizEsparsa M; int k;
    while (scanf("%d %d", &n, &k)>0) {
        if (n!=-1) {
            M.N = n;
            M.linhas = (No **) malloc(sizeof(No *)*M.N);
            for (int i=0; i<M.N; i++) { M.linhas[i] = NULL; };
            u = NULL; int ul = 0;
            for (int i=0; i<k; i++) {
                scanf("%d %d %d", &l, &c, &e);
                if (l != ul) {
                    u = NULL; ul = l;
                }
                InsereEnc(&M.linhas[l],c,e,&u);
            }
        } else {
            M.N = 1000;
            M.linhas = (No **) malloc(sizeof(No *)*M.N);
            for (int i=0; i<M.N; i++) { M.linhas[i] = NULL; };
            for (l=0; l<10; l++) {
                u = NULL;
                for (c=0; c<10; c++) {
                    InsereEnc(&M.linhas[l],c,1,&u);
                }
            }
        }
        MatrizEsparsa M2 = AoQuadrado(M);
        Escreve(M2); DestroiMatriz(M2); DestroiMatriz(M);
    }
    return 0;
}
