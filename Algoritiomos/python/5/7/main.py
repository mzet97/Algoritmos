import sys

sys.setrecursionlimit(200000)

class No:
    def __init__(self, e):
        self.E = e
        self.Esq = None
        self.Dir = None

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

def PreencheCheiaBusca(counter, h):
    if h == 0:
        return None
    T = No(0)
    T.Esq = PreencheCheiaBusca(counter, h - 1)
    T.E = counter[0]
    counter[0] += 1
    T.Dir = PreencheCheiaBusca(counter, h - 1)
    return T

def Maior(T):
    if T is None:
        return -1
    # In a BST, the maximum is in the rightmost node
    if T.Dir is None:
        return T.E
    return Maior(T.Dir)

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
            print(Maior(T))
        else:
            # teste automatico
            counter = [1]
            T = PreencheCheiaBusca(counter, 20)
            for _ in range(100000):
                Maior(T)
            print(Maior(T))

if __name__ == "__main__":
    main()
