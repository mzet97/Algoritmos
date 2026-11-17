import sys

def entrada():
    for line in sys.stdin:
        line = line.strip()
        if line:
            yield line

def Soma(A, n, B, m):
    R = []
    carry = 0
    i = n - 1
    j = m - 1
    while i >= 0 or j >= 0 or carry > 0:
        s = carry
        if i >= 0:
            s += A[i]
            i -= 1
        if j >= 0:
            s += B[j]
            j -= 1
        R.append(s % 10)
        carry = s // 10
    R.reverse()
    return R

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
    R = Soma(A, n, B, m)
    print(' '.join(str(d) for d in R))
