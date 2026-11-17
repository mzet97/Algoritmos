#include <stdio.h>
#include <stdlib.h>

void Soma(int A[], int n, int B[], int m, int R[], int *k) {
    int carry = 0;
    int i = n - 1, j = m - 1;
    *k = 0;
    int tmp[2000];
    int tk = 0;
    while (i >= 0 || j >= 0 || carry > 0) {
        int sum = carry;
        if (i >= 0) { sum += A[i]; i--; }
        if (j >= 0) { sum += B[j]; j--; }
        tmp[tk++] = sum % 10;
        carry = sum / 10;
    }
    *k = tk;
    for (int p = 0; p < tk; p++) {
        R[p] = tmp[tk - 1 - p];
    }
}

void Imprime(int R[], int k) {
    for (int i = 0; i < k; i++) {
        printf("%d ", R[i]);
    }
    printf("\n");
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
        Soma(A, n, B, m, R, &k);
        Imprime(R, k);
    }
    free(A); free(B); free(R);
}
