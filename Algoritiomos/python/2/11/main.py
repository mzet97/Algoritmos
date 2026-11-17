import sys

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def Remove(L, p):
    if p.Prox is not None:
        p.E = p.Prox.E
        p.Prox = p.Prox.Prox
    else:
        # p is the last node; need to find predecessor
        if L is p:
            # Only node - can't really remove in O(1)
            pass
        else:
            prev = L
            while prev.Prox is not p:
                prev = prev.Prox
            prev.Prox = None

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
    print("]")

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    while idx < len(tokens):
        n = int(tokens[idx]); idx += 1
        L = None
        V = []
        if n != -1:
            for i in range(n):
                e = int(tokens[idx]); idx += 1
                L = Insere(L, e)
                V.append(L)
            ri = int(tokens[idx]); idx += 1
            Remove(L, V[ri - 1])
            Escreve(L)
        else:
            n = 100000
            V = []
            for i in range(n):
                L = Insere(L, i % 100)
                V.append(L)
            for i in range(10):
                Remove(L, V[i * 9000])
            Escreve(L)

if __name__ == "__main__":
    main()
