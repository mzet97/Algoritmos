import sys

def entrada():
    for line in sys.stdin:
        line = line.strip()
        if line:
            yield line

def Subtrai(A, n, B, m):
    R = [0] * n
    emprestimo = 0
    j = m - 1
    for i in range(n - 1, -1, -1):
        diff = A[i] - emprestimo
        if j >= 0:
            diff -= B[j]
            j -= 1
        if diff < 0:
            diff += 10
            emprestimo = 1
        else:
            emprestimo = 0
        R[i] = diff
    # Remove leading zeros
    k = 0
    while k < len(R) - 1 and R[k] == 0:
        k += 1
    return R[k:]

gen = entrada()
for line in gen:
    parts = line.split()
    n = int(parts[0])
    m = int(parts[1])
    A = []
    for _ in range(n):
        A.append(int(next(gen)))
    B = []
    for _ in range(m):
        B.append(int(next(gen)))
    R = Subtrai(A, n, B, m)
    print(' '.join(str(d) for d in R))
