#include <cstdio>
#include <cstdlib>
#include <algorithm>

typedef struct No {
	int E;
	No * Prox;
} No;

No * Soma2a2(No * L) {
	if (L == NULL || L->Prox == NULL) return NULL;
	No * rest = Soma2a2(L->Prox->Prox);
	No * novo = (No *) malloc(sizeof(No));
	novo->E = L->E + L->Prox->E;
	novo->Prox = rest;
	return novo;
}

void Insere(No * &L, int e) {
	No * novo = (No *) malloc(sizeof(No));
	novo->E = e; novo->Prox = L; L = novo;
}

void Escreve(No * L) {
	int n = 0;
	No * p = L;
	while (p != NULL) {
		n++; p = p->Prox;
	}
	int * Lv = (int *) malloc(sizeof(int)*n);
	p = L; int i = 0;
	while (p != NULL) {
		Lv[i] = p->E; i++; p = p->Prox;
	}
	std::sort(Lv,Lv+n);
	printf("[ "); for (int i=0; i<n; i++) printf("%d ", Lv[i]); printf("]\n");
	free(Lv);
}

void Destroi(No * &L) {
	No * p, * pr; p = L;
	while (p != NULL) {
		pr = p; p = p->Prox;
		free(pr);
	}
	L = NULL;
}

No * C_Soma2a2(No * L, int q=1) {
	No * r;
	for (int t=0; t<q; t++) {
		r = Soma2a2(L);
	}
	return r;
}

int main() {
	setbuf(stdout, NULL); setbuf(stderr, NULL);
	int n; No * L = NULL; No * Lr;
	while (scanf("%d", &n)>0) {
		if (n!=-1) {
			/* lê n, e1, e2, ..., en */
			for (int i=0; i<n; i++) {
				int e; scanf("%d", &e);	Insere(L,e);
			}
			Lr = C_Soma2a2(L); Escreve(Lr); Destroi(Lr); Destroi(L);
		} else {
			/* automatic test */
			n = 1000;
			for (int i=0; i<n; i++) {
				Insere(L,i);
			}
			Lr = C_Soma2a2(L); Escreve(Lr); Destroi(Lr); Destroi(L);
		}
	}
	return 0;
}
