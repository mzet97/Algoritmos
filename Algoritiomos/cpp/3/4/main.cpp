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

typedef struct Parentese {
	char desc; /* '(' ou ')' */
	int tipo;
} Parentese;

bool VerificaParentizacao(Parentese E[], int n) {
	Pilha P; Constroi(P);
	for (int i = 0; i < n; i++) {
		if (E[i].desc == '(') {
			Empilha(P, E[i].tipo);
		} else {
			if (Tamanho(P) == 0) return false;
			int tp = Desempilha(P);
			if (tp != E[i].tipo) return false;
		}
	}
	bool ok = (Tamanho(P) == 0);
	Destroi(P);
	return ok;
}

int main() {
	setbuf(stdout, NULL); setbuf(stderr, NULL);
	Parentese * E; int n;
	while (scanf("%d", &n)>0) {
		if (n>=0) {
			/* teste manual: 'n <parentização com n elementos>' */
			char * Ec = (char *) malloc(sizeof(char)*(4*n+1));
			E = (Parentese *) malloc(sizeof(Parentese)*n);
			if (n > 0) {
				char fmt[32];
				snprintf(fmt, sizeof(fmt), "%%%ds", 4*n);
				scanf(fmt, Ec);
			} else {
				Ec[0] = '\0';
			}
			int j=0;
			for (int i=0; i<n; i++) {
				E[i].desc = Ec[j]; j++;
				char tipo[10];
				int z=j;
				while ((Ec[j] != '\0') && (Ec[j] != '(') && (Ec[j] != ')')) {
					tipo[j-z] = Ec[j];
					j = j+1;
				}
				tipo[j-z] = '\0';
				E[i].tipo = atoi(tipo);
			}
			printf("%d\n", VerificaParentizacao(E,n));
			free(E); free(Ec);
		} else {
			/* teste automático */
			int nlin = n;
			n = 1000000;
			E = (Parentese *) malloc(sizeof(Parentese)*n);
			if (nlin==-1) {
				for (int i=0; i<n/2; i++) {	E[i].desc = '('; E[i].tipo = 1;	}
				for (int i=n/2; i<n; i++) {	E[i].desc = ')'; E[i].tipo = 1;	}
			} else if (nlin==-2) {
				for (int i=0; i<n/2; i++) {	E[i].desc = '('; E[i].tipo = i+1;	}
				for (int i=n/2; i<n; i++) {	E[i].desc = ')'; E[i].tipo = n-(i+1)+1;	}
			} else if (nlin==-3) {
				for (int i=0; i<n; i += 2) { E[i].desc = '('; E[i].tipo = i+1; E[i+1].desc = ')'; E[i+1].tipo = i+1; }
			} else if (nlin==-4) {
				for (int i=0; i<n/2-1; i++) {	E[i].desc = '('; E[i].tipo = 1;	}
				for (int i=n/2-1; i<n; i++) {	E[i].desc = ')'; E[i].tipo = 1; }
			} else if (nlin==-5) {
				for (int i=0; i<n/2+1; i++) {	E[i].desc = '('; E[i].tipo = 1;	}
				for (int i=n/2+1; i<n; i++) {	E[i].desc = ')'; E[i].tipo = 1; }
			}
			printf("%d\n", VerificaParentizacao(E,n));
			free(E);
		}
	}

	return 0;
}
