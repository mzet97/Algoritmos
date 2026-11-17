#include "../bib1.h"
#include <math.h>

void tv(int d, int pl, int ph) {
    double diag_cm = d * 2.54;
    double ratio = (double)pl / ph;
    double h = diag_cm / sqrt(1 + ratio*ratio);
    double w = h * ratio;
    w = round(w * 10) / 10;
    h = round(h * 10) / 10;
    printf("(");
    print_python_float(w);
    printf(", ");
    print_python_float(h);
    printf(")\n");
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int d = atoi(buf);
        if (!entrada(buf, sizeof(buf))) break;
        int pl = atoi(buf);
        if (!entrada(buf, sizeof(buf))) break;
        int ph = atoi(buf);
        tv(d, pl, ph);
    }
    return 0;
}
