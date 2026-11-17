import sys

sys.setrecursionlimit(200000)

class No:
    def __init__(self, e):
        self.E = e
        self.Esq = None
        self.Dir = None

def EhArvoreBusca(T):
    def check(T, lo, hi):
        if T is None:
            return True
        if T.E <= lo or T.E >= hi:
            return False
        return check(T.Esq, lo, T.E) and check(T.Dir, T.E, hi)
    return 1 if check(T, float('-inf'), float('inf')) else 0

def Insere(T, e, idx):
    i = idx[0]
    if e[i] == 0:
        i += 1
        idx[0] = i
        return None
    T = No(e[i])
    i += 1
    idx[0] = i
    T.Esq = Insere(T.Esq, e, idx)
    T.Dir = Insere(T.Dir, e, idx)
    return T

def InsereBal(T, i, j):
    if i <= j:
        T = No(0)
        m = (i + j) // 2
        T.E = m
        if m == 900000:
            T.E = 1
        T.Esq = InsereBal(T.Esq, i, m - 1) if i <= m - 1 else None
        T.Dir = InsereBal(T.Dir, m + 1, j) if m + 1 <= j else None
        return T
    return None

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        val = int(tokens[i]); i += 1
        T = None
        if val > -1:
            # teste manual: visita preordem de T (0 para nulo)
            e = [val]
            while e[-1] != -1:
                e.append(int(tokens[i])); i += 1
            idx = [0]
            T = Insere(T, e, idx)
        else:
            # teste automatico
            T = InsereBal(None, 1, 900000)
        print(EhArvoreBusca(T))

if __name__ == "__main__":
    main()
