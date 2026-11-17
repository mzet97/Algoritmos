#include <cstdio>
#include <cstring>

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
        if (n > 0) {
            char fmt[32];
            snprintf(fmt, sizeof(fmt), "%%%ds", n + 599999);
            scanf(fmt, A);
        } else {
            A[0] = '\0';
        }
        PermutacaoCircular(A, n, k);
        printf("%s\n", A);
        free(A);
    }
}
