#include "../bib1.h"
#include <stdlib.h>

void soma_lista(const int *x, int xlen, const int *y, int ylen, int **out, int *outlen) {
    int len = xlen > ylen ? xlen : ylen;
    int *r = malloc((len + 1) * sizeof(int));
    int carry = 0, ri = 0;
    for (int i = 0; i < len; i++) {
        int a = i < xlen ? x[i] : 0;
        int b = i < ylen ? y[i] : 0;
        int s = a + b + carry;
        r[ri++] = s % 10;
        carry = s / 10;
    }
    if (carry) r[ri++] = carry;
    *out = r;
    *outlen = ri;
}

int main(void) {
    char buf[2000002];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        char xstr[1000002], ystr[1000002];
        if (strcmp(buf, "@entradagrande") == 0) {
            memset(xstr, '9', 1000000); xstr[1000000] = '\0';
            memset(ystr, '9', 1000000); ystr[1000000] = '\0';
        } else {
            strcpy(xstr, buf);
            if (!entrada(ystr, sizeof(ystr))) break;
        }
        int xlen = strlen(xstr), ylen = strlen(ystr);
        int *xl = malloc(xlen * sizeof(int)), *yl = malloc(ylen * sizeof(int));
        for (int i = 0; i < xlen; i++) xl[xlen-1-i] = xstr[i] - '0';
        for (int i = 0; i < ylen; i++) yl[ylen-1-i] = ystr[i] - '0';
        int *r, rlen;
        soma_lista(xl, xlen, yl, ylen, &r, &rlen);
        for (int i = rlen-1; i >= 0; i--) putchar('0' + r[i]);
        putchar('\n');
        free(xl); free(yl); free(r);
    }
    return 0;
}
