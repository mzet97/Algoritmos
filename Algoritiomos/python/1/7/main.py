import sys

def entrada():
    for line in sys.stdin:
        line = line.strip()
        if line:
            yield line

def ConcatenaPrefixos(A, B, n, m):
    C = []
    for i in range(n):
        C.append(A[i])
    for i in range(m):
        C.append(B[i])
    return C

gen = entrada()
for line in gen:
    parts = line.split()
    n = int(parts[0])
    m = int(parts[1])
    A = next(gen)
    B = next(gen)
    C = ConcatenaPrefixos(A, B, n, m)
    print(''.join(C))
