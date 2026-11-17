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

int Tamanho(Pilha &P) {
	return P.N;
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

void Constroi(Pilha &P) {
	P.Topo = NULL; P.N = 0;
}

void Destroi(Pilha &P) {
	while (Tamanho(P) > 0) {
		Desempilha(P);
	}
}

int SomaSequenciaOperacoes(int S[], int n) {
	Pilha P; Constroi(P);
	for (int i = 0; i < n; i++) {
		if (S[i] != 0) {
			Empilha(P, S[i]);
		} else {
			if (Tamanho(P) > 0) {
				Desempilha(P);
			}
		}
	}
	int soma = 0;
	while (Tamanho(P) > 0) {
		soma += Desempilha(P);
	}
	Destroi(P);
	return soma;
}

int main() {
	setbuf(stdout, NULL); setbuf(stderr, NULL);
	int n; int * S;
	while (scanf("%d", &n) > 0) {
		if (n != -1) {
			S = (int *) malloc(sizeof(int)*n);
			for (int i = 0; i < n; i++) {
				scanf("%d", &S[i]);
			}
		} else {
			/* automatic test */
			n = 1000000;
			S = (int *) malloc(sizeof(int)*n);
			for (int i = 0; i < n/2; i++) {
				S[i] = 1; S[n/2+i] = 0;
			}
		}
		printf("%d\n", SomaSequenciaOperacoes(S, n));
		free(S);
	}
	return 0;
}
