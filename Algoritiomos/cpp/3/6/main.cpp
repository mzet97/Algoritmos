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

void ManobrarVagoes(int n, int V[]) {
	Pilha M; Constroi(M);
	Fila entrada; Constroi(entrada);
	// Vagões chegam na ordem n,n-1,...,2,1 (1 é o primeiro, n é o último)
	// V[0] é o último vagão desejado, V[n-1] é o primeiro
	// A fila representa os vagões que chegam, do primeiro ao último
	for (int i = n-1; i >= 0; i--) {
		Enfileira(entrada, i+1);
	}
	int idx = 0; // índice no vetor V (V[0] é o próximo a sair por último... V[n-1] primeiro)
	// Na verdade V[0] é o último vagão, V[n-1] é o primeiro vagão de saída
	// Vamos processar: precisamos gerar V[n-1] como primeiro a sair, ..., V[0] como último
	// Inverter: queremos sair V[n-1], V[n-2], ..., V[0]
	// Vamos usar uma fila de saída também
	int outIdx = n - 1; // próximo vagão que deve sair

	while (outIdx >= 0) {
		// Tentar tirar da área de manobra
		if (Tamanho(M) > 0 && Topo(M) == V[outIdx]) {
			printf("M=>S\n");
			Desempilha(M);
			outIdx--;
			continue;
		}
		// Tentar trazer da entrada para manobra
		if (Tamanho(entrada) > 0) {
			int v = Desenfileira(entrada);
			if (v == V[outIdx]) {
				// E=>M, M=>S
				printf("E=>M\n");
				printf("M=>S\n");
				outIdx--;
			} else {
				printf("E=>M\n");
				Empilha(M, v);
			}
			continue;
		}
		// Se chegou aqui, não é possível (mas não deveria acontecer para entradas válidas)
		break;
	}
	Destroi(M); Destroi(entrada);
}

int main() {
	setbuf(stdout, NULL); setbuf(stderr, NULL);
	int n; int * V;
	while (scanf("%d", &n) > 0) {
		if (n != -1) {
			V = (int *) malloc(sizeof(int) * n);
			for (int i = 0; i < n; i++) {
				scanf("%d", &V[i]);
			}
		} else {
			/* automatic test */
			n = 1000;
			V = (int *) malloc(sizeof(int) * n);
			int M = n; int m = 1;
			for (int i = n-1; i >= 0; i = i-2) {
				V[i] = M; V[i-1] = m;
				M = M-1; m = m+1;
			}
		}
		ManobrarVagoes(n, V);
		free(V);
	}
	return 0;
}
