import sys

sys.setrecursionlimit(100000)

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def Inverte(L):
    if L is None or L.Prox is None:
        return L
    r = Inverte(L.Prox)
    L.Prox.Prox = L
    L.Prox = None
    return r

def Insere(L, e):
    novo = No(e)
    novo.Prox = L
    return novo

def Escreve(L):
    p = L
    sys.stdout.write("[ ")
    while p is not None:
        sys.stdout.write(str(p.E) + " ")
        p = p.Prox
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
            Lr = Inverte(L)
            Escreve(Lr)
        else:
            # automatic test
            n = 1000
            for j in range(n):
                L = Insere(L, j % 100)
            for _ in range(1000):
                Lr = Inverte(L)
            Lr = Inverte(L)
            Escreve(Lr)

if __name__ == "__main__":
    main()
