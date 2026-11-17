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

def Escrever(T):
    if T is not None:
        sys.stdout.write("(Q=%d,E=%d " % (T.Q, T.E))
        sys.stdout.write("(")
        Escrever(T.Esq)
        sys.stdout.write(")")
        sys.stdout.write("(")
        Escrever(T.Dir)
        sys.stdout.write("))")

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        n = int(tokens[i]); i += 1
        T = None
        if n > 0:
            # teste manual
            for j in range(n):
                x = int(tokens[i]); i += 1
                T = Insere(T, x)
        else:
            # teste automatico
            T = InsereBal(T, 1, 50000)
            T = InsereBal(T, 1, 50000)
        Escrever(T)
        print()

if __name__ == "__main__":
    main()
