#include "../bib1.h"
#include <stdlib.h>

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int a = atoi(buf);
        if (!entrada(buf, sizeof(buf))) break;
        int b = atoi(buf);
        if (a == 0) {
            printf("None\n");
        } else {
            double r = (double)(-b) / (double)a;
            print_python_float(r); printf("\n");
        }
    }
    return 0;
}
