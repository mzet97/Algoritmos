#include <cstdio>
#include <cstdlib>

typedef struct No {
	int E;
	No * Prox;
} No;

typedef struct Pilha {
	No * Topo;
	int N;
} Pilha;

void Constroi(Pilha &P) {
	P.Topo = NULL; P.N = 0;
}

void Empilha(Pilha &P, int x) {
	No * n = (No *) malloc(sizeof(No));
	n->E = x;
	n->Prox = P.Topo;
	P.Topo = n;
	P.N++;
}

int Desempilha(Pilha &P) {
	No * n = P.Topo; int x = n->E;
	P.Topo = P.Topo->Prox;
	free(n);
	P.N--;
	return x;
}

int Topo(Pilha &P) {
	return P.Topo->E;
}

int Tamanho(Pilha &P) {
	return P.N;
}

void Destroi(Pilha &P) {
	while (Tamanho(P) > 0) {
		Desempilha(P);
	}
}

bool PilhasIdenticas(Pilha &p1, Pilha &p2) {
	if (Tamanho(p1) != Tamanho(p2)) return false;
	Pilha aux1, aux2;
	Constroi(aux1); Constroi(aux2);
	bool iguais = true;
	while (Tamanho(p1) > 0) {
		int a = Desempilha(p1);
		int b = Desempilha(p2);
		if (a != b) iguais = false;
		Empilha(aux1, a);
		Empilha(aux2, b);
	}
	// Restore original stacks
	while (Tamanho(aux1) > 0) {
		Empilha(p1, Desempilha(aux1));
		Empilha(p2, Desempilha(aux2));
	}
	Destroi(aux1); Destroi(aux2);
	return iguais;
}

int main() {
	setbuf(stdout, NULL); setbuf(stderr, NULL);
	int n; Pilha p1; Pilha p2;
	while (scanf("%d", &n) > 0) {
		if (n != -1) {
			int e;
			Constroi(p1); Constroi(p2);
			for (int i = 0; i < n; i++) {
				scanf("%d", &e); Empilha(p1, e);
			}
			scanf("%d", &n);
			for (int i = 0; i < n; i++) {
				scanf("%d", &e); Empilha(p2, e);
			}
		} else {
			/* automatic test */
			n = 1000000;
			Constroi(p1); Constroi(p2);
			for (int i = 0; i < n; i++) {
				Empilha(p1, i+1); Empilha(p2, i+1);
			}
		}
		printf("%d\n", PilhasIdenticas(p1, p2));
		Destroi(p1); Destroi(p2);
	}
	return 0;
}
