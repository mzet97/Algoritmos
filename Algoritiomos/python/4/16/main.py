import sys

sys.setrecursionlimit(100000)

def RemoveMaiores(L, n, k):
    # Remove elementos maiores que k, recursivamente
    # L is a list, n is current length (mutable reference via list wrapper)
    if len(L) == 0:
        return
    # Process from end to beginning to avoid index issues
    _remove_maiores_rec(L, len(L) - 1, k)

def _remove_maiores_rec(L, idx, k):
    if idx < 0:
        return
    _remove_maiores_rec(L, idx - 1, k)
    while idx < len(L) and L[idx] > k:
        L.pop(idx)

def Escreve(L):
    Lv = sorted(L[:])
    sys.stdout.write("[")
    for v in Lv:
        sys.stdout.write(str(v))
    sys.stdout.write("]\n")

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        n = int(tokens[i]); i += 1
        if n != -1:
            # n, e1, e2, ..., en, k
            L = []
            for j in range(n):
                L.append(int(tokens[i])); i += 1
            k = int(tokens[i]); i += 1
            RemoveMaiores(L, n, k)
            Escreve(L)
        else:
            # automatic test
            nlin = 50000
            k = 2
            L = []
            for t in range(20):
                L = [(2 if j % 2 == 0 else 3) for j in range(nlin)]
                RemoveMaiores(L, nlin, k)
                if t == 0:
                    Escreve(L)

if __name__ == "__main__":
    main()
