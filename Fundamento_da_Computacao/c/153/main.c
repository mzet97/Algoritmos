#include "../bib1.h"

int raiz(long long n, long long inf, long long sup) {
    while (sup - inf > 1) {
        long long mid = (inf + sup) / 2;
        if (mid * mid <= n) inf = mid;
        else sup = mid;
    }
    return (int)inf;
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        long long n = atoll(buf);
        printf("%d\n", raiz(n, 0, n+1));
    }
    return 0;
}
