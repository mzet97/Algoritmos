#include <cstdio>
#include <cstdlib>

long long int CalculaSoma(int n, int m[], int k) {
    long long int soma = 0;
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j < k; j++) {
            if (i % m[j] == 0) {
                soma += i;
                break;
            }
        }
    }
    return soma;
}

int main() {
    int n; int k;
    int *m;
    while (scanf("%d %d", &n, &k) > 0) {
        m = (int *)malloc(sizeof(int) * n);
        for (int i = 0; i < k; i++) {
            scanf("%d", &m[i]);
        }
        printf("%lld\n", CalculaSoma(n, m, k));
        free(m);
    }
}
