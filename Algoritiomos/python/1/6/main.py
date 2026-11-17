import sys

def entrada():
    for line in sys.stdin:
        line = line.strip()
        if line:
            yield line

def Maior(A, n):
    maior = A[0]
    for i in range(1, n):
        if A[i] > maior:
            maior = A[i]
    return maior

gen = entrada()
for line in gen:
    n = int(line)
    A = []
    for _ in range(n):
        A.append(int(next(gen)))
    print(Maior(A, n))
