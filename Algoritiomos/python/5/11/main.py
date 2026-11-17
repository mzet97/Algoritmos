import sys

sys.setrecursionlimit(200000)

class No:
    def __init__(self, e, q=1):
        self.E = e
        self.Q = q
        self.Esq = None
        self.Dir = None

def Insere(T, x):
    if T is None:
        return No(x)
    if T.E > x:
        T.Esq = Insere(T.Esq, x)
        T.Q += 1
    else:
        T.Dir = Insere(T.Dir, x)
        T.Q += 1
    return T

def InsereBal(T, i, j):
    if i <= j:
        m = (i + j) // 2
        T = Insere(T, m)
        T = InsereBal(T, i, m - 1)
        T = InsereBal(T, m + 1, j)
    return T

def QuantidadeMenorIgual(T, a):
    if T is None:
        return 0
    if T.E == a:
        if T.Esq is None:
            return 0
        return T.Esq.Q
    elif T.E > a:
        return QuantidadeMenorIgual(T.Esq, a)
    else:
        # T.E < a: all of left subtree + T itself + those in right subtree <= a
        q_esq = T.Esq.Q if T.Esq else 0
        return 1 + q_esq + QuantidadeMenorIgual(T.Dir, a)

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        n = int(tokens[i]); i += 1
        T = None
        a = 0
        if n > 0:
            # teste manual
            for j in range(n):
                x = int(tokens[i]); i += 1
                T = Insere(T, x)
            a = int(tokens[i]); i += 1
        else:
            # teste automatico
            T = InsereBal(T, 1, 500000)
            a = 358254
        for _ in range(100000):
            QuantidadeMenorIgual(T, a)
        print(QuantidadeMenorIgual(T, a))

if __name__ == "__main__":
    main()
