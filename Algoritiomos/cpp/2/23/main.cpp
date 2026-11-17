#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>

typedef struct NoEnc {
	int Chave;
	int E;
	NoEnc * Prox;
} NoEnc;

typedef struct NoSeq {
	int Chave;
	int E;
} NoSeq;

NoEnc * ConverterSeqParaEnc(NoSeq L[], int N) {
	NoEnc * result = NULL;
	NoEnc * u = NULL;
	for (int i = 0; i < N; i++) {
		NoEnc * novo = (NoEnc *) malloc(sizeof(NoEnc));
		novo->Chave = L[i].Chave;
		novo->E = L[i].E;
		novo->Prox = NULL;
		if (u == NULL) {
			result = novo;
		} else {
			u->Prox = novo;
		}
		u = novo;
	}
	return result;
}

NoSeq * ConverterEncParaSeq(NoEnc * L, int &N) {
	// Count elements
	int count = 0;
	NoEnc * p = L;
	while (p != NULL) {
		count++;
		p = p->Prox;
	}
	N = count;
	NoSeq * result = (NoSeq *) malloc(sizeof(NoSeq) * N);
	p = L;
	for (int i = 0; i < N; i++) {
		result[i].Chave = p->Chave;
		result[i].E = p->E;
		p = p->Prox;
	}
	return result;
}

void InsereEnc(NoEnc * &L, int c, int e, NoEnc * &u) {
	NoEnc * novo = (NoEnc *) malloc(sizeof(NoEnc));
	novo->Chave = c; novo->E = e; novo->Prox = NULL;
	if (u == NULL) {
		L = novo;
	} else {
		u->Prox = novo;
	}
	u = novo;
}

void EscreveEnc(NoEnc * L) {
	int n = 0;
	NoEnc * p = L;
	printf("[");
	while (p != NULL) { printf("c=%d;e=%d ", p->Chave, p->E); p = p->Prox; };
	printf("]\n");
}

void Destroi(NoEnc * &L) {
	NoEnc * p, * pr; p = L;
	while (p != NULL) {
		pr = p; p = p->Prox;
		free(pr);
	}
}


int main() {
	setbuf(stdout, NULL); setbuf(stderr, NULL);
	int n; NoEnc * L; NoEnc * u; int c; int e; int N; NoSeq * Lconv1; NoEnc * Lconv2;
	while (scanf("%d", &n)>0) {
		L = NULL; u = NULL;
		if (n!=-1) {
			/* lê c1, i1, c2, i2, ..., cn, in */
			for (int i=0; i<n; i++) {
				scanf("%d %d", &c, &e); InsereEnc(L,c,e,u);
			}
			Lconv1 = ConverterEncParaSeq(L, N);
			Lconv2 = ConverterSeqParaEnc(Lconv1, N);
			EscreveEnc(Lconv2); free(Lconv1); Destroi(Lconv2); Destroi(L);
		} else {
			/* automatic test */
			n = 500000;
			for (int i=0; i<n; i++) {
				InsereEnc(L,i,n+i,u);
			}
			Lconv1 = ConverterEncParaSeq(L, N);
			Lconv2 = ConverterSeqParaEnc(Lconv1, N);
			EscreveEnc(Lconv2); free(Lconv1); Destroi(Lconv2); Destroi(L);
		}
	}
	return 0;
}
