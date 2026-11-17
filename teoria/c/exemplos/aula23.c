#include <stddef.h>
#include <stdio.h>
#include <string.h>

enum { LINE_CAP = 16 };

typedef struct {
    char bytes[LINE_CAP];
    size_t len;
} Parser;

static int feed(Parser *p, const unsigned char *chunk, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        if (chunk[i] == 0) return -1;
        if (p->len == LINE_CAP - 1) return -1;
        p->bytes[p->len++] = (char)chunk[i];
        if (chunk[i] == '\n') {
            p->bytes[p->len] = '\0';
            printf("linha: %s", p->bytes);
            p->len = 0;
        }
    }
    return 0;
}

int main(void) {
    Parser p = {0};
    const unsigned char first[] = "PI";
    const unsigned char second[] = "NG\nECHO oi\nPAR";
    const unsigned char third[] = "CIAL\n";
    if (feed(&p, first, sizeof(first) - 1) != 0 ||
        feed(&p, second, sizeof(second) - 1) != 0 ||
        feed(&p, third, sizeof(third) - 1) != 0) return 1;
    if (p.len != 0) return 1;
    puts("tres linhas completas");
    return 0;
}
