#include "../bib1.h"

int contagem(int x, int y) {
    char s[32];
    snprintf(s, sizeof(s), "%d", x < 0 ? -x : x);
    char target = '0' + ((y < 0 ? -y : y) % 10);
    int count = 0;
    for (int i = 0; s[i]; i++)
        if (s[i] == target) count++;
    return count;
}

int main(void) {
    char buf[256];
    while (1) {
        if (!entrada(buf, sizeof(buf))) break;
        int x = atoi(buf);
        if (!entrada(buf, sizeof(buf))) break;
        int y = atoi(buf);
        printf("%d\n", contagem(x, y));
    }
    return 0;
}
