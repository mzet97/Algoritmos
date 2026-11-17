#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reverse(char A[], int l, int r) {
    while (l < r) {
        char tmp = A[l];
        A[l] = A[r];
        A[r] = tmp;
        l++;
        r--;
    }
}

void PermutacaoCircular(char A[], int n, int k) {
    if (n == 0) return;
    k = k % n;
    if (k == 0) return;
    reverse(A, 0, n - 1);
    reverse(A, 0, k - 1);
    reverse(A, k, n - 1);
}

int main() {
    int n, k;
    char *A;
    while (scanf("%d %d", &n, &k) > 0) {
        A = (char *)malloc(sizeof(char) * (n + 600000));
        scanf("%s", A);
        PermutacaoCircular(A, n, k);
        printf("%s\n", A);
        free(A);
    }
}
