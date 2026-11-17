#include <cstdio>
#include <cstring>

typedef struct E1 {
    char Nome[10];
    int Idade;
} E1;

typedef struct E2 {
    int Turma;
    struct E1 *Aluno;
    struct E2 *Prox;
} E2;

E1 V[3];
E2 *L;

void TestarMemoria(int t) {
    if (t==1) printf("%d %d %d\n", V[0].Idade, V[1].Idade, V[2].Idade);
    if (t==2) printf("%c %c %c %c\n", V[0].Nome[0], V[0].Nome[1], V[0].Nome[2], V[0].Nome[3]);
    if (t==3) printf("%c %c %c\n", V[1].Nome[0], V[1].Nome[1], V[1].Nome[2]);
    if (t==4) printf("%c %c %c\n", V[2].Nome[0], V[2].Nome[1], V[2].Nome[2]);
    if (t==5) printf("%d %d %d %d\n", L->Turma, L->Prox->Turma, L->Prox->Prox->Turma, L->Prox->Prox->Prox->Turma);
    if (t==6) printf("%d\n", L->Prox->Prox->Prox->Prox == NULL ? 1 : 0);
    if (t==7) printf("%d %d %d %d\n", L->Aluno->Idade, L->Prox->Aluno->Idade, L->Prox->Prox->Aluno->Idade, L->Prox->Prox->Prox->Aluno->Idade);
}

int main() {
    strcpy(V[0].Nome, "JOAO"); V[0].Idade = 5;
    strcpy(V[1].Nome, "LEO");  V[1].Idade = 8;
    strcpy(V[2].Nome, "LIA");  V[2].Idade = 9;

    E2 *n4 = (E2 *)malloc(sizeof(E2)); n4->Turma = 2; n4->Aluno = &V[2]; n4->Prox = NULL;
    E2 *n3 = (E2 *)malloc(sizeof(E2)); n3->Turma = 2; n3->Aluno = &V[1]; n3->Prox = n4;
    E2 *n2 = (E2 *)malloc(sizeof(E2)); n2->Turma = 1; n2->Aluno = &V[2]; n2->Prox = n3;
    E2 *n1 = (E2 *)malloc(sizeof(E2)); n1->Turma = 1; n1->Aluno = &V[0]; n1->Prox = n2;
    L = n1;

    int t;
    while (scanf("%d", &t) > 0) {
        TestarMemoria(t);
    }

    free(n1); free(n2); free(n3); free(n4);
}
