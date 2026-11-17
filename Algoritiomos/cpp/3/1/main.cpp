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
	if (P.Topo == NULL) return -1;
	No * n = P.Topo; int x = n->E;
	P.Topo = P.Topo->Prox;
	free(n);
	P.N--;
	return x;
}

int Topo(Pilha &P) {
	if (P.Topo == NULL) return -1;
	return P.Topo->E;
}

int Tamanho(Pilha &P) {
	return P.N;
}

typedef struct Fila {
	Pilha P1,P2;
} Fila;

void Constroi(Fila &F) {
	Constroi(F.P1); Constroi(F.P2);
}
void Enfileira(Fila &F, int x) {
	Empilha(F.P1, x);
}

int Desenfileira(Fila &F) {
	if (Tamanho(F.P2) == 0) {
		while (Tamanho(F.P1) > 0) {
			Empilha(F.P2, Desempilha(F.P1));
		}
	}
	if (Tamanho(F.P2) == 0) return -1;
	return Desempilha(F.P2);
}

int Proximo(Fila &F) {
	if (Tamanho(F.P2) == 0) {
		while (Tamanho(F.P1) > 0) {
			Empilha(F.P2, Desempilha(F.P1));
		}
	}
	if (Tamanho(F.P2) == 0) return -1;
	return Topo(F.P2);
}

int Tamanho(Fila &F) {
	return Tamanho(F.P1) + Tamanho(F.P2);
}

int main() {
	setbuf(stdout, NULL); setbuf(stderr, NULL);
	Fila F; Constroi(F);
	char op[2]; int e;
	while (scanf("%1s", op)>0) {
		/* teste manual: 'i e' = enfileira o elemento e; 'r' = desenfileira, 'p' = consulta o prÃ³ximo, 'n' = consulta o tamanho */
		if (op[0] == 'i') {
			scanf("%d", &e); Enfileira(F,e);
		} else if (op[0] == 'r') {
			printf("%d\n", Desenfileira(F));
		} else if (op[0] == 'p') {
			printf("%d\n", Proximo(F));
		} else if (op[0] == 'n') {
			printf("%d\n", Tamanho(F));
		} else {
			/* teste automÃ¡tico */
			for (int i=1; i<=1000; i++) {
				Enfileira(F,i);
			}
			while (Tamanho(F) > 0) {
				printf("%d ", Tamanho(F));
				printf("%d ", Proximo(F));
				printf("%d ", Desenfileira(F));
			}
			printf("\n");
		}
	}

	return 0;
}

