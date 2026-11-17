#include "../bib1.h"

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int a = atoi(buf);
        if (!entrada(buf, sizeof(buf))) break;
        int q = atoi(buf);
        if (!entrada(buf, sizeof(buf))) break;
        int n = atoi(buf);
        printf("[");
        long long val = a;
        for (int i = 0; i < n; i++) {
            if (i > 0) printf(", ");
            printf("%lld", val);
            val *= q;
        }
        printf("]\n");
    }
    return 0;
}
