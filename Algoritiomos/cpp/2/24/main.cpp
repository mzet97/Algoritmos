#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>

typedef struct No {
	int col;
	int E;
	No * Prox;
} No;

typedef struct MatrizEsparsa {
	int N;
	No ** linhas; //linhas[i] uma lista encadeada dos valores
	              //não nulos da matriz na linha i
} MatrizEsparsa;



MatrizEsparsa AoQuadrado(MatrizEsparsa M) {
	MatrizEsparsa R;
	R.N = M.N;
	R.linhas = (No **) malloc(sizeof(No *) * R.N);
	for (int i = 0; i < R.N; i++) {
		R.linhas[i] = NULL;
	}

	for (int i = 0; i < M.N; i++) {
		for (int j = 0; j < M.N; j++) {
			// Compute R[i][j] = sum over k of M[i][k] * M[k][j]
			int sum = 0;
			No * pi = M.linhas[i];
			while (pi != NULL) {
				int k = pi->col;
				// Check if M[k][j] exists (is non-zero)
				No * pk = M.linhas[k];
				while (pk != NULL) {
					if (pk->col == j) {
						sum += pi->E * pk->E;
						break;
					}
					pk = pk->Prox;
				}
				pi = pi->Prox;
			}
			if (sum != 0) {
				No * novo = (No *) malloc(sizeof(No));
				novo->col = j;
				novo->E = sum;
				novo->Prox = R.linhas[i];
				R.linhas[i] = novo;
			}
		}
	}
	return R;
}

void InsereEnc(No * &L, int col, int e, No * &u) {
	No * novo = (No *) malloc(sizeof(No));
	novo->col = col; novo->E = e; novo->Prox = NULL;
	if (u == NULL) {
		L = novo;
	} else {
		u->Prox = novo;
	}
	u = novo;
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

void Destroi(No * L) {
	No * p, * pr; p = L;
	while (p != NULL) {
		pr = p; p = p->Prox;
		free(pr);
	}
}

void Destroi(MatrizEsparsa M) {
	for (int i=0; i<M.N; i++) {
		Destroi(M.linhas[i]);
	};
}

int main() {
	setbuf(stdout, NULL); setbuf(stderr, NULL);
	int n; No * L; No * u; int l; int c; int e; MatrizEsparsa M; int k;
	while (scanf("%d %d", &n, &k)>0) {
		if (n!=-1) {
			M.N = n;
			M.linhas = (No **) malloc(sizeof(No *)*M.N);
			for (int i=0; i<M.N; i++) { M.linhas[i] = NULL; };
			/* lê linha, coluna, valor, ...  (k trios) */
			u = NULL; int ul = 0;
			for (int i=0; i<k; i++) {
				scanf("%d %d %d", &l, &c, &e);
				if (l != ul) {
					u = NULL; ul = l;
				}
				InsereEnc(M.linhas[l],c,e,u);
			}
		} else {
			/* automatic test */
			M.N = 1000;
			M.linhas = (No **) malloc(sizeof(No *)*M.N);
			for (int i=0; i<M.N; i++) { M.linhas[i] = NULL; };
			for (int l=0; l<10; l++) {
				u = NULL;
				for (int c=0; c<10; c++) {
					InsereEnc(M.linhas[l],c,1,u);
				}
			}
		}
		MatrizEsparsa M2 = AoQuadrado(M);
		Escreve(M2); Destroi(M2); Destroi(M);
	}
	return 0;
}
