import sys

class No:
    def __init__(self, e):
        self.E = e
        self.Ant = None
        self.Prox = None

def Remove(L, p):
    if p.Ant is not None:
        p.Ant.Prox = p.Prox
    else:
        L = p.Prox
    if p.Prox is not None:
        p.Prox.Ant = p.Ant
    return L

def Insere(L, e):
    novo = No(e)
    if L is None:
        novo.Prox = None
        novo.Ant = None
        L = novo
    else:
        novo.Prox = L
        novo.Ant = None
        L.Ant = novo
        L = novo
    return L

def Escreve(L):
    p = L
    pUlt = None
    sys.stdout.write("Esq >> Dir: [ ")
    while p is not None:
        sys.stdout.write(str(p.E) + " ")
        pUlt = p
        p = p.Prox
    sys.stdout.write("]; ")
    sys.stdout.write("Dir >> Esq: [ ")
    p = pUlt
    while p is not None:
        sys.stdout.write(str(p.E) + " ")
        p = p.Ant
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
            L = Remove(L, V[ri - 1])
            Escreve(L)
        else:
            n = 1000000
            V = []
            for i in range(n):
                L = Insere(L, i % 100)
                V.append(L)
            for i in range(1000):
                L = Remove(L, V[i * 900])
            Escreve(L)

if __name__ == "__main__":
    main()
