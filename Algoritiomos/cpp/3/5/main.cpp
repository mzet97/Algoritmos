#include <cstdio>
#include <cstdlib>

typedef struct NoFila {
	int E;
	NoFila * Prox;
} NoFila;

typedef struct Fila {
	NoFila * Prox;
	NoFila * Ult;
	int N;
} Fila;

void Constroi(Fila &F) {
	F.Prox = NULL; F.Ult = NULL; F.N = 0;
}

void Enfileira(Fila &F, int x) {
	NoFila * n = (NoFila *) malloc(sizeof(NoFila));
	n->E = x;
	n->Prox = NULL;
	if (F.Ult == NULL) {
		F.Prox = n;
		F.Ult = n;
	} else {
		F.Ult->Prox = n;
		F.Ult = n;
	}
	F.N++;
}

int Desenfileira(Fila &F) {
	NoFila * n = F.Prox; int x = n->E;
	F.Prox = F.Prox->Prox;
	if (F.Prox == NULL) F.Ult = NULL;
	free(n);
	F.N--;
	return x;
}

int Proximo(Fila &F) {
	return F.Prox->E;
}

int Tamanho(Fila &F) {
	return F.N;
}

void Destroi(Fila &F) {
	while (Tamanho(F) > 0) {
		Desenfileira(F);
	}
}

void LerEscreverKUltimos(int k) {
	Fila F; Constroi(F);
	int x;
	while (scanf("%d", &x) > 0 && x != 0) {
		Enfileira(F, x);
		if (Tamanho(F) > k) {
			Desenfileira(F);
		}
	}
	while (Tamanho(F) > 0) {
		printf("%d ", Desenfileira(F));
	}
	printf("\n");
	Destroi(F);
}

int main() {
	setbuf(stdout, NULL); setbuf(stderr, NULL);
	int k;
	while (scanf("%d", &k)>0) {
		/* teste manual: 'k <sequência de elementos, último igual a 0>' */
		LerEscreverKUltimos(k);
	}
	return 0;
}
