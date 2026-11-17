#include <stdio.h>
#include <stdlib.h>

int RaizQuadAux(int n, int lo, int hi) {
    if (lo > hi) return hi;
    int mid = lo + (hi - lo) / 2;
    long long sq = (long long)mid * mid;
    if (sq == n) return mid;
    if (sq < n) return RaizQuadAux(n, mid + 1, hi);
    return RaizQuadAux(n, lo, mid - 1);
}

int RaizQuad(int n) {
    return RaizQuadAux(n, 0, n);
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int n;
    while (scanf("%d", &n) > 0) {
        if (n >= 0) {
            printf("%d\n", RaizQuad(n));
        } else {
            n = 50000;
            for (int t = 1; t <= 200; t++) RaizQuad(n);
            printf("%d\n", RaizQuad(n));
        }
    }
    return 0;
}
