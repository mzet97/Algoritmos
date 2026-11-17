#include <stdio.h>
#include <stdlib.h>

int Divisores(int n, int k) {
    if (k == 0) return 0;
    int c = Divisores(n, k-1);
    if (n % k == 0) c++;
    return c;
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int n; int k;
    while (scanf("%d %d", &n, &k) > 0) {
        if (n >= 0) {
            printf("%d\n", Divisores(n, k));
        } else {
            n = 50000; k = 50000;
            for (int t = 1; t <= 200; t++) Divisores(n, k);
            printf("%d\n", Divisores(n, k));
        }
    }
    return 0;
}
