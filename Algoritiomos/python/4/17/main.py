import sys

sys.setrecursionlimit(100000)

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def Insere(L, e):
    novo = No(e)
    novo.Prox = L
    return novo

def RemoveMaiores(L, k):
    if L is None:
        return None
    L.Prox = RemoveMaiores(L.Prox, k)
    if L.E > k:
        return L.Prox
    return L

def Escreve(L):
    n = 0
    p = L
    while p is not None:
        n += 1
        p = p.Prox
    Lv = []
    p = L
    while p is not None:
        Lv.append(p.E)
        p = p.Prox
    Lv.sort()
    sys.stdout.write("[")
    for v in Lv:
        sys.stdout.write(str(v))
    sys.stdout.write("]\n")

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        max_n = int(tokens[i]); i += 1
        if max_n != -1:
            # le n, e1, e2, ..., en, k
            L = None
            for j in range(max_n):
                e = int(tokens[i]); i += 1
                L = Insere(L, e)
            k = int(tokens[i]); i += 1
            L = RemoveMaiores(L, k)
            Escreve(L)
        else:
            # automatic test
            for t in range(20):
                L = None
                max_n = 50000
                for j in range(max_n):
                    L = Insere(L, 2 if j % 2 == 0 else 3)
                L = RemoveMaiores(L, 2)
                if t == 0:
                    Escreve(L)

if __name__ == "__main__":
    main()
