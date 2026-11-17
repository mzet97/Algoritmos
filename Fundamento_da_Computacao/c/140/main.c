#include "../bib1.h"

int perfeito(int n) {
    if (n < 2) return 0;
    int soma = 1;
    for (int i = 2; (long long)i * i <= n; i++) {
        if (n % i == 0) {
            soma += i;
            if (i != n / i) soma += n / i;
        }
    }
    return soma == n;
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int n = atoi(buf);
        printf("%s\n", perfeito(n) ? "True" : "False");
    }
    return 0;
}
