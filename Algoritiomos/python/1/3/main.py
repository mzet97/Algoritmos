def entrada():
    import sys
    for line in sys.stdin:
        line = line.strip()
        if line:
            yield line

def CalculaSoma(n, m, k):
    soma = 0
    for i in range(1, n + 1):
        for j in range(k):
            if i % m[j] == 0:
                soma += i
                break
    return soma

gen = entrada()
for line in gen:
    parts = line.split()
    n = int(parts[0])
    k = int(parts[1])
    m = []
    for _ in range(k):
        m.append(int(next(gen)))
    print(CalculaSoma(n, m, k))
