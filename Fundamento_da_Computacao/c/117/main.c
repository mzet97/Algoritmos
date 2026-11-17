#include "../bib1.h"
#include <stdio.h>

void cientifico(double n) {
    if (n == 0.0) { printf("0.0x10^0\n"); return; }
    int sinal = 0;
    if (n < 0) { sinal = 1; n = -n; }
    int exp = 0;
    while (n >= 10) { n /= 10; exp++; }
    while (n < 1) { n *= 10; exp--; }
    if (sinal) printf("-");
    printf("%.1fx10^%d\n", n, exp);
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        double n = atof(buf);
        cientifico(n);
    }
    return 0;
}
