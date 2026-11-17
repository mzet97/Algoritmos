import sys

sys.setrecursionlimit(200000)

class No:
    def __init__(self, e):
        self.E = e
        self.Esq = None
        self.Dir = None

def Maior(T):
    if T is None:
        return -1
    m = T.E
    me = Maior(T.Esq)
    md = Maior(T.Dir)
    if me > m:
        m = me
    if md > m:
        m = md
    return m

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

def PreencheCheia(h):
    if h == 0:
        return None
    T = No(h)
    T.Esq = PreencheCheia(h - 1)
    T.Dir = PreencheCheia(h - 1)
    return T

def PreencheZigZag(h):
    if h == 0:
        return None
    T = No(h)
    T.Esq = PreencheZigZag(h - 1)
    T.Dir = None
    return T

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        val = int(tokens[i]); i += 1
        T = None
        if val > -2:
            # teste manual: visita preordem de T (0 para nulo)
            e = [val]
            while e[-1] != -1:
                e.append(int(tokens[i])); i += 1
            idx = [0]
            T = Insere(T, e, idx)
            print(Maior(T))
        else:
            # teste automatico
            if val == -2:
                T = PreencheCheia(19)
            else:
                T = PreencheZigZag(50000)
            print(Maior(T))

if __name__ == "__main__":
    main()
