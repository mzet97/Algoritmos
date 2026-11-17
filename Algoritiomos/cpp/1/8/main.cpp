#include <cstdio>
#include <cstdlib>

int Fibonacci(int n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    const int MOD = 1000000000;
    long long a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        long long c = (a + b) % MOD;
        a = b;
        b = c;
    }
    return (int)b;
}

int main() {
    int n;
    while (scanf("%d", &n) > 0) {
        printf("%d\n", Fibonacci(n));
    }
}
