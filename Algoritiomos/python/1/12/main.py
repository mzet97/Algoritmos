import sys

def entrada():
    for line in sys.stdin:
        line = line.strip()
        if line:
            yield line

def PermutacaoCircular(A, n, k):
    if n == 0:
        return A
    k = k % n
    if k == 0:
        return A
    return A[:k][::-1] + A[k:][::-1]  # Wait, this is wrong. Let me think.
    # Actually: right circular shift by k positions
    # Reverse entire, reverse first k, reverse remaining

def PermutacaoCircularCorreta(A_list, n, k):
    if n == 0:
        return
    k = k % n
    if k == 0:
        return
    # Reverse entire array
    A_list[:n] = A_list[:n][::-1]
    # Reverse first k
    A_list[:k] = A_list[:k][::-1]
    # Reverse remaining
    A_list[k:n] = A_list[k:n][::-1]

gen = entrada()
for line in gen:
    parts = line.split()
    n = int(parts[0])
    k = int(parts[1])
    A = list(next(gen))
    PermutacaoCircularCorreta(A, n, k)
    print(''.join(A))
