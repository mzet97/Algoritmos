#include <stdio.h>
#include <stdlib.h>

typedef struct No {
	int E;
	No * Prox;
} No;

int KUltimo(No * L, int k) {
	// Count elements (excluding sentinel)
	if (L == NULL || k <= 0) return -1;
	int n = 0;
	No * p = L->Prox;
	while (p != L) {
		n++;
		p = p->Prox;
	}
	if (k > n) return -1;
	// Find the (n-k+1)-th element (1-indexed) from start
	p = L->Prox;
	for (int i = 1; i < n - k + 1; i++) {
		p = p->Prox;
	}
	return p->E;
}

void Constroi(No * &L, No * &u) {
	L = (No *) malloc(sizeof(No));
	u = L;
	L->Prox = L;
}

void Insere(No * &L, int e, No * &u) {
	No * novo = (No *) malloc(sizeof(No));
	novo->E = e;
	novo->Prox = u->Prox;
	u->Prox = novo;
	u = novo;
}

int main() {
	setbuf(stdout, NULL); setbuf(stderr, NULL);
	No * L = NULL; No * u; Constroi(L,u);
	char op[2]; int e, k;
	while (scanf("%1s", op)>0) {
		if (op[0] != 'a') {
			/* teste manual: i e = insere o elemento e; b k = busca o k-Ãºltimo */
			if (op[0] == 'i') {
				scanf("%d", &e); Insere(L,e,u);
			} else {
				scanf("%d", &k); printf("%d\n", KUltimo(L,k));
			}
		} else {
			/* teste automÃ¡tico */
			int n = 2000000;
			for (int i=1; i<=n; i++) {
				Insere(L,i,u);
			}
			printf("%d\n", KUltimo(L,n/2));
		}
	}
	return 0;
}

