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

bool FilasIdenticas(Fila &f1, Fila &f2) {
	if (Tamanho(f1) != Tamanho(f2)) return false;
	Fila aux1, aux2;
	Constroi(aux1); Constroi(aux2);
	bool iguais = true;
	while (Tamanho(f1) > 0) {
		int a = Desenfileira(f1);
		int b = Desenfileira(f2);
		if (a != b) iguais = false;
		Enfileira(aux1, a);
		Enfileira(aux2, b);
	}
	// Restore original queues
	while (Tamanho(aux1) > 0) {
		Enfileira(f1, Desenfileira(aux1));
		Enfileira(f2, Desenfileira(aux2));
	}
	Destroi(aux1); Destroi(aux2);
	return iguais;
}

int main() {
	setbuf(stdout, NULL); setbuf(stderr, NULL);
	int n; Fila f1; Fila f2;
	while (scanf("%d", &n) > 0) {
		if (n != -1) {
			int e;
			Constroi(f1); Constroi(f2);
			for (int i = 0; i < n; i++) {
				scanf("%d", &e); Enfileira(f1, e);
			}
			scanf("%d", &n);
			for (int i = 0; i < n; i++) {
				scanf("%d", &e); Enfileira(f2, e);
			}
		} else {
			/* automatic test */
			int e; n = 1000000;
			Constroi(f1); Constroi(f2);
			for (int i = 0; i < n; i++) {
				Enfileira(f1, i+1); Enfileira(f2, i+1);
			}
		}
		printf("%d\n", FilasIdenticas(f1, f2));
		Destroi(f1); Destroi(f2);
	}
	return 0;
}
