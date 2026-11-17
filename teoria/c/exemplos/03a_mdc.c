#include <assert.h>
#include <stdio.h>

static unsigned int mdc(unsigned int a, unsigned int b) {
    while (b != 0U) {
        unsigned int resto = a % b;
        a = b;
        b = resto;
    }
    return a;
}

int main(void) {
    assert(mdc(54U, 24U) == 6U);
    assert(mdc(0U, 24U) == 24U);
    assert(mdc(24U, 0U) == 24U);
    assert(mdc(0U, 0U) == 0U);

    printf("mdc(54, 24) = %u\n", mdc(54U, 24U));
    return 0;
}
