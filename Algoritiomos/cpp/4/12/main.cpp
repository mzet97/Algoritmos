#include <cstdio>
#include <cstdlib>

typedef struct No {
	int E;
	No * Prox;
} No;

int KUltimoRec(No * L, int k, int &pos) {
	if (L == NULL) {
		pos = 0;
		return -1;
	}
	int val = KUltimoRec(L->Prox, k, pos);
	pos++;
	if (pos == k) return L->E;
	return val;
}

int KUltimo(No * L, int k) {
	if (k <= 0) return -1;
	int pos = 0;
	return KUltimoRec(L, k, pos);
}

void Constroi(No * &L, No * &u) {
	L = NULL; u = NULL;
}

void Insere(No * &L, int e, No * &u) {
	No * novo = (No *) malloc(sizeof(No));
	novo->E = e;
	novo->Prox = NULL;
	if (L == NULL) {
		L = novo;
	} else {
		u->Prox = novo;
	}
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
			int n = 50000;
			for (int i=1; i<=n; i++) {
				Insere(L,i,u);
			}
			for (int t=1; t<=100; t++) KUltimo(L,n/2);
			printf("%d\n", KUltimo(L,n/2));
		}
	}
	return 0;
}

