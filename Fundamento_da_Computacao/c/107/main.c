#include "../bib1.h"
#include <stdlib.h>

void multiplica_lista(const int *x, int xlen, int y, int **out, int *outlen) {
    int cap = xlen + 20;
    int *r = malloc(cap * sizeof(int));
    int carry = 0, ri = 0;
    for (int i = 0; i < xlen; i++) {
        int p = x[i] * y + carry;
        r[ri++] = p % 10;
        carry = p / 10;
    }
    while (carry > 0) {
        r[ri++] = carry % 10;
        carry /= 10;
    }
    if (ri == 0) r[ri++] = 0;
    *out = r;
    *outlen = ri;
}

int main(void) {
    char buf[1000002];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        char *xstr = strdup(buf);
        if (!entrada(buf, sizeof(buf))) { free(xstr); break; }
        int y = atoi(buf);
        int xlen = strlen(xstr);
        int *xl = malloc(xlen * sizeof(int));
        for (int i = 0; i < xlen; i++) xl[xlen-1-i] = xstr[i] - '0';
        int *r, rlen;
        multiplica_lista(xl, xlen, y, &r, &rlen);
        for (int i = rlen-1; i >= 0; i--) putchar('0' + r[i]);
        putchar('\n');
        free(xl); free(r); free(xstr);
    }
    return 0;
}
