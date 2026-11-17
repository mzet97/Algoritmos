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

def Soma2a2(L):
    if L is None or L.Prox is None:
        return None
    novo = No(L.E + L.Prox.E)
    novo.Prox = Soma2a2(L.Prox.Prox)
    return novo

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
    sys.stdout.write("[ ")
    for v in Lv:
        sys.stdout.write(str(v) + " ")
    sys.stdout.write("]\n")

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        n = int(tokens[i]); i += 1
        L = None
        if n != -1:
            # le n, e1, e2, ..., en
            for j in range(n):
                e = int(tokens[i]); i += 1
                L = Insere(L, e)
            Lr = Soma2a2(L)
            Escreve(Lr)
        else:
            # automatic test
            n = 1000
            for j in range(n):
                L = Insere(L, j)
            Lr = Soma2a2(L)
            Escreve(Lr)

if __name__ == "__main__":
    main()
