#include <stdio.h>
#include <stdlib.h>

long long int Bin(int n, int k) {
    if (k == 0 || k == n) return 1;
    return Bin(n-1, k-1) + Bin(n-1, k);
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int n; int k;
    while (scanf("%d %d", &n, &k) > 0) {
        if (n >= 0) {
            printf("%lld\n", Bin(n, k));
        } else {
            n = 50000; k = 49999;
            for (int t = 1; t <= 200; t++) Bin(n, k);
            printf("%lld\n", Bin(n, k));
        }
    }
    return 0;
}
