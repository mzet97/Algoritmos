#include <cstdio>
#include <cstdlib>

long long int CalculaSoma(int n) {
    return (long long int)n * (n + 1) / 2;
}

int main() {
    int n;
    while (scanf("%d", &n) > 0) {
        printf("%lld\n", CalculaSoma(n));
    }
}
