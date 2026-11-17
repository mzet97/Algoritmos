import sys

def entrada():
    for line in sys.stdin:
        line = line.strip()
        if line:
            yield line

def ContaMisteriosa(n):
    limite = 10 ** n
    soma = 0.0
    for k in range(1, limite + 1):
        sinal = 1.0 if (k + 1) % 2 == 0 else -1.0
        soma += sinal / (2 * k - 1)
    return 4.0 * soma

for line in entrada():
    n = int(line)
    print(f"{ContaMisteriosa(n):.8f}")
