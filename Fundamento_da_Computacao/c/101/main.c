#include "../bib1.h"
#include <stdlib.h>

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int x = atoi(buf);
        if (!entrada(buf, sizeof(buf))) break;
        int y = atoi(buf);
        if (!entrada(buf, sizeof(buf))) break;
        char op = buf[0];

        if (op == '+') {
            printf("%d\n", x + y);
        } else if (op == '-') {
            printf("%d\n", x - y);
        } else if (op == 'x') {
            printf("%d\n", x * y);
        } else if (op == '/') {
            /* Python / always returns float */
            double r = (double)x / (double)y;
            print_python_float(r); printf("\n");
        }
    }
    return 0;
}
