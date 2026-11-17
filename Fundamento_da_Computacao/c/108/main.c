#include "../bib1.h"
#include <stdlib.h>
#include <string.h>

void multiplica_lista_lista(const int *x, int xlen, const int *y, int ylen, int **out, int *outlen) {
    int cap = xlen + ylen + 2;
    int *r = calloc(cap, sizeof(int));
    for (int i = 0; i < xlen; i++) {
        int carry = 0;
        for (int j = 0; j < ylen; j++) {
            int p = r[i+j] + x[i]*y[j] + carry;
            r[i+j] = p % 10;
            carry = p / 10;
        }
        int k = i + ylen;
        while (carry > 0) {
            int p = r[k] + carry;
            r[k] = p % 10;
            carry = p / 10;
            k++;
        }
    }
    int ri = xlen + ylen;
    while (ri > 1 && r[ri-1] == 0) ri--;
    *out = r;
    *outlen = ri;
}

int main(void) {
    char buf[2000002];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        char xstr[2002], ystr[2002];
        if (strcmp(buf, "@entradagrande") == 0) {
            memset(xstr, '9', 1000); xstr[1000] = '\0';
            memset(ystr, '9', 1000); ystr[1000] = '\0';
        } else {
            strcpy(xstr, buf);
            if (!entrada(ystr, sizeof(ystr))) break;
        }
        int xlen = strlen(xstr), ylen = strlen(ystr);
        int *xl = malloc(xlen * sizeof(int)), *yl = malloc(ylen * sizeof(int));
        for (int i = 0; i < xlen; i++) xl[xlen-1-i] = xstr[i] - '0';
        for (int i = 0; i < ylen; i++) yl[ylen-1-i] = ystr[i] - '0';
        int *r, rlen;
        multiplica_lista_lista(xl, xlen, yl, ylen, &r, &rlen);
        for (int i = rlen-1; i >= 0; i--) putchar('0' + r[i]);
        putchar('\n');
        free(xl); free(yl); free(r);
    }
    return 0;
}
