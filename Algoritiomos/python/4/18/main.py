import sys

sys.setrecursionlimit(100000)

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def KUltimo(L, k):
    if L is None:
        return -1
    # Count elements
    n = _count(L)
    if k > n:
        return -1
    # Walk to (n-k)-th element (0-indexed)
    p = L
    for _ in range(n - k):
        p = p.Prox
    return p.E

def _count(L):
    if L is None:
        return 0
    return 1 + _count(L.Prox)

def Insere(L, u, e):
    novo = No(e)
    if L is None:
        L = novo
    else:
        u.Prox = novo
    u = novo
    return L, u

def main():
    L = None
    u = None
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        op = tokens[i]; i += 1
        if op[0] != 'a':
            # teste manual: i e = insere; b k = busca k-ultimo
            if op[0] == 'i':
                e = int(tokens[i]); i += 1
                L, u = Insere(L, u, e)
            else:
                k = int(tokens[i]); i += 1
                print(KUltimo(L, k))
        else:
            # teste automatico
            n = 50000
            for j in range(1, n + 1):
                L, u = Insere(L, u, j)
            for _ in range(100):
                KUltimo(L, n // 2)
            print(KUltimo(L, n // 2))

if __name__ == "__main__":
    main()
