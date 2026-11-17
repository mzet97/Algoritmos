#include "../bib1.h"
#include <stdlib.h>
#include <string.h>

int maiorigual_lista(int *x, int xlen, int *y, int ylen) {
    /* Compare numbers represented as digit lists (least significant first) */
    /* First compare lengths (longer number is bigger, since no leading zeros assumed) */
    if (xlen != ylen) return xlen > ylen;
    /* Same length: compare from most significant digit */
    for (int i = xlen - 1; i >= 0; i--) {
        if (x[i] != y[i]) return x[i] > y[i];
    }
    /* Equal */
    return 1;
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        char xstr[256];
        strcpy(xstr, buf);
        if (!entrada(buf, sizeof(buf))) break;
        char ystr[256];
        strcpy(ystr, buf);

        int xlen = strlen(xstr);
        int ylen = strlen(ystr);

        /* Build digit lists: reversed string, so x_lista[0] = least significant digit */
        int x_lista[256], y_lista[256];
        for (int i = 0; i < xlen; i++)
            x_lista[i] = xstr[xlen - 1 - i] - '0';
        for (int i = 0; i < ylen; i++)
            y_lista[i] = ystr[ylen - 1 - i] - '0';

        if (maiorigual_lista(x_lista, xlen, y_lista, ylen))
            printf("1\n");
        else
            printf("0\n");
    }
    return 0;
}
