#include <stdio.h>
#include <stdlib.h>

int RaizQuad(int n) {
    if (n == 0 || n == 1) return n;
    int r = RaizQuad(n/4);
    if ((2*r+1)*(2*r+1) <= n) return 2*r+1;
    return 2*r;
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
