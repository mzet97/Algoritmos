import sys

def entrada():
    for line in sys.stdin:
        line = line.strip()
        if line:
            yield line

def RepresentacaoDecimal(n):
    if n == 0:
        return [0]
    digitos = []
    while n > 0:
        digitos.append(n % 10)
        n //= 10
    digitos.reverse()
    return digitos

for line in entrada():
    n = int(line)
    R = RepresentacaoDecimal(n)
    print(' '.join(str(d) for d in R))
