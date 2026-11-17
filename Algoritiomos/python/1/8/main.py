import sys

def entrada():
    for line in sys.stdin:
        line = line.strip()
        if line:
            yield line

def Fibonacci(n):
    if n == 0:
        return 0
    if n == 1:
        return 1
    MOD = 1000000000
    a, b = 0, 1
    for _ in range(2, n + 1):
        a, b = b, (a + b) % MOD
    return b

for line in entrada():
    n = int(line)
    print(Fibonacci(n))
