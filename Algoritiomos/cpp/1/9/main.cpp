#include <cstdio>
#include <cstdlib>

void RepresentacaoDecimal(long long int n, int R[], int &k) {
    if (n == 0) {
        R[0] = 0;
        k = 1;
        return;
    }
    k = 0;
    int tmp[20];
    while (n > 0) {
        tmp[k++] = n % 10;
        n /= 10;
    }
    for (int i = 0; i < k; i++) {
        R[i] = tmp[k - 1 - i];
    }
}

int main() {
    long long int n;
    int R[16], k;
    while (scanf("%lld", &n) > 0) {
        RepresentacaoDecimal(n, R, k);
        for (int i = 0; i < k; i++) {
            printf("%d ", R[i]);
        }
        printf("\n");
    }
}
