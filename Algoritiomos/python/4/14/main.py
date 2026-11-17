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

def SemRepeticoes(L):
    if L is None:
        return None
    r = SemRepeticoes(L.Prox)
    # Check if L.E is already in r
    p = r
    found = False
    while p is not None:
        if p.E == L.E:
            found = True
            break
        p = p.Prox
    if found:
        return r
    novo = No(L.E)
    novo.Prox = r
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
    sys.stdout.write("[")
    for v in Lv:
        sys.stdout.write(str(v))
    sys.stdout.write("]\n")

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        n = int(tokens[i]); i += 1
        L = None
        if n > -1:
            # le n, e1, e2, ..., en
            for j in range(n):
                e = int(tokens[i]); i += 1
                L = Insere(L, e)
            Lr = SemRepeticoes(L)
            Escreve(Lr)
        else:
            # automatic test
            if n == -1:
                n = 1000
                for j in range(n):
                    L = Insere(L, 2 if j % 2 == 0 else 3)
                Lr = SemRepeticoes(L)
                Escreve(Lr)
            else:
                n = 1000
                for j in range(n):
                    L = Insere(L, j)
                Lr = SemRepeticoes(L)
                Escreve(Lr)

if __name__ == "__main__":
    main()
