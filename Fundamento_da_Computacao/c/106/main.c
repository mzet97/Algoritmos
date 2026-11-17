#include "../bib1.h"
#include <stdlib.h>
#include <string.h>

void subtrai_lista(const int *x, int xlen, const int *y, int ylen, int **out, int *outlen) {
    int *r = malloc(xlen * sizeof(int));
    int borrow = 0, ri = xlen;
    for (int i = 0; i < xlen; i++) {
        int a = x[i];
        int b = i < ylen ? y[i] : 0;
        a -= borrow;
        if (a < b) { a += 10; borrow = 1; } else { borrow = 0; }
        r[i] = a - b;
    }
    while (ri > 1 && r[ri-1] == 0) ri--;
    *out = r;
    *outlen = ri;
}

int main(void) {
    char buf[2000002];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        char xstr[1000002], ystr[1000002];
        if (strcmp(buf, "@entradagrande1") == 0) {
            memset(xstr, '1', 1000000); xstr[1000000] = '\0';
            memset(ystr, '9', 999999); ystr[999999] = '\0';
        } else if (strcmp(buf, "@entradagrande2") == 0) {
            memset(xstr, '1', 1000000); xstr[1000000] = '\0';
            memset(ystr, '1', 1000000); ystr[1000000] = '\0';
        } else {
            strcpy(xstr, buf);
            if (!entrada(ystr, sizeof(ystr))) break;
        }
        int xlen = strlen(xstr), ylen = strlen(ystr);
        int *xl = malloc(xlen * sizeof(int)), *yl = malloc(ylen * sizeof(int));
        for (int i = 0; i < xlen; i++) xl[xlen-1-i] = xstr[i] - '0';
        for (int i = 0; i < ylen; i++) yl[ylen-1-i] = ystr[i] - '0';
        int *r, rlen;
        subtrai_lista(xl, xlen, yl, ylen, &r, &rlen);
        for (int i = rlen-1; i >= 0; i--) putchar('0' + r[i]);
        putchar('\n');
        free(xl); free(yl); free(r);
    }
    return 0;
}
