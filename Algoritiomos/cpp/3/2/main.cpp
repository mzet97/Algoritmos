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

void AlternarPosNeg(int num[], int n) {
	Fila pos, neg;
	Constroi(pos); Constroi(neg);
	for (int i = 0; i < n; i++) {
		if (num[i] >= 0) {
			Enfileira(pos, num[i]);
		} else {
			Enfileira(neg, num[i]);
		}
	}
	while (Tamanho(pos) > 0 || Tamanho(neg) > 0) {
		if (Tamanho(pos) > 0) {
			printf("%d ", Desenfileira(pos));
		}
		if (Tamanho(neg) > 0) {
			printf("%d ", Desenfileira(neg));
		}
	}
	printf("\n");
	Destroi(pos); Destroi(neg);
}

int main() {
	setbuf(stdout, NULL); setbuf(stderr, NULL);
	int * num; int n;
	while (scanf("%d", &n)>0) {
		if (n > -1) {
			/* teste manual: n, num[0..n-1] */
			num = (int *) malloc(sizeof(int) * n);
			for (int i=0; i<n; i++) {
				scanf("%d", &num[i]);
			}
			AlternarPosNeg(num, n);
			free(num);
		} else {
			/* teste automático */
			if (n==-1) {
				n = 100000;
				num = (int *) malloc(sizeof(int) * n);
				int s = 1;
				for (int i=0; i<n; i++) { num[i] = s*(i+1); s = -s; }
				AlternarPosNeg(num, n);
				free(num);
			} else if (n==-2) {
				n = 100000;
				num = (int *) malloc(sizeof(int) * n);
				for (int i=0; i<n/2; i++) num[i] = (i+1);
				for (int i=n/2; i<n; i++) num[i] = -(i+1);
				AlternarPosNeg(num, n);
				free(num);
			} else {
				n = 100000;
				num = (int *) malloc(sizeof(int) * n);
				for (int i=0; i<n/2; i++) num[i] = -(i+1);
				for (int i=n/2; i<n; i++) num[i] = (i+1);
				AlternarPosNeg(num, n);
				free(num);
			}
		}
	}

	return 0;
}
