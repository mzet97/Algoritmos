def entrada():
    import sys
    for line in sys.stdin:
        line = line.strip()
        if line:
            yield line

def CalculaSoma(n):
    soma = 0
    for i in range(1, n + 1):
        soma += i
    return soma

for line in entrada():
    n = int(line)
    print(CalculaSoma(n))
