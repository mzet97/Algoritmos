#include <cstdio>
#include <cstdlib>

void Imprime(int R[], int k) {
    for (int i = 0; i < k; i++) {
        printf("%d ", R[i]);
    }
    printf("\n");
}

void Subtrai(int A[], int n, int B[], int m, int R[], int &k) {
    int emprestimo = 0;
    int j = m - 1;
    for (int i = n - 1; i >= 0; i--) {
        int diff = A[i] - emprestimo;
        if (j >= 0) {
            diff -= B[j];
            j--;
        }
        if (diff < 0) {
            diff += 10;
            emprestimo = 1;
        } else {
            emprestimo = 0;
        }
        R[i] = diff;
    }
    k = n;
    while (k > 1 && R[0] == 0) {
        for (int i = 0; i < k - 1; i++) {
            R[i] = R[i + 1];
        }
        k--;
    }
}

int main() {
    int n, m;
    int *A, *B, *R;
    int k;
    A = (int *)malloc(sizeof(int) * 1000);
    B = (int *)malloc(sizeof(int) * 1000);
    R = (int *)malloc(sizeof(int) * 1000 * 1000);
    while (scanf("%d %d", &n, &m) > 0) {
        for (int i = 0; i < n; i++) {
            scanf("%d", &A[i]);
        }
        for (int i = 0; i < m; i++) {
            scanf("%d", &B[i]);
        }
        Subtrai(A, n, B, m, R, k);
        Imprime(R, k);
    }
    free(A); free(B); free(R);
}
