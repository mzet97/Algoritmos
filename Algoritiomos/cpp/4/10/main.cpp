#include <cstdio>
#include <cstdlib>

typedef struct No {
	int E;
	No * Prox;
} No;

int Maior(No * L) {
	if (L == NULL) return -1;
	if (L->Prox == NULL) return L->E;
	int m = Maior(L->Prox);
	if (L->E > m) return L->E;
	return m;
}

void Insere(No * &L, int e) {
	No * novo = (No *) malloc(sizeof(No));
	novo->E = e;
	novo->Prox = L;
	L = novo;
}

int main() {
	setbuf(stdout, NULL); setbuf(stderr, NULL);
	No * L = NULL;
	char op[2]; int e, k;
	while (scanf("%1s", op)>0) {
		if (op[0] != 'a') {
			/* teste manual: i e = insere o elemento e; m = busca o maior */
			if (op[0] == 'i') {
				scanf("%d", &e); Insere(L,e);
			} else {
				printf("%d\n", Maior(L));
			}
		} else {
			/* teste automÃ¡tico */
			int n = 50000;
			for (int i=n/2; i<=n; i++) {
				Insere(L,i);
			}
			for (int i=1; i < n/2; i++) {
				Insere(L,i);
			}
			for (int t=1; t<=100; t++)  Maior(L);
			printf("%d\n", Maior(L));
		}
	}
	return 0;
}

