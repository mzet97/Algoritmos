#include "../bib1.h"

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int h1 = atoi(buf);
        if (!entrada(buf, sizeof(buf))) break;
        int m1 = atoi(buf);
        if (!entrada(buf, sizeof(buf))) break;
        int h2 = atoi(buf);
        if (!entrada(buf, sizeof(buf))) break;
        int m2 = atoi(buf);

        int s1 = h1 * 3600 + m1 * 60;
        int s2 = h2 * 3600 + m2 * 60;
        int diff = s2 - s1;
        if (diff < 0) diff = -diff;

        printf("%d\n", diff);
    }
    return 0;
}
