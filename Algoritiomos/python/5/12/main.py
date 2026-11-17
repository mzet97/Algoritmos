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

def QuantidadeIntervalo(T, a, b):
    if T is None:
        return 0
    if T.E < a:
        return QuantidadeIntervalo(T.Dir, a, b)
    elif T.E > b:
        return QuantidadeIntervalo(T.Esq, a, b)
    else:
        # a <= T.E <= b
        q_esq = T.Esq.Q if T.Esq else 0
        q_dir = T.Dir.Q if T.Dir else 0
        # Those in left subtree that are >= a, and right subtree that are <= b
        left_count = _count_ge(T.Esq, a)
        right_count = _count_le(T.Dir, b)
        return 1 + left_count + right_count

def _count_ge(T, a):
    if T is None:
        return 0
    if T.E == a:
        return 1 + (T.Dir.Q if T.Dir else 0)
    elif T.E < a:
        return _count_ge(T.Dir, a)
    else:
        q_dir = T.Dir.Q if T.Dir else 0
        return 1 + q_dir + _count_ge(T.Esq, a)

def _count_le(T, b):
    if T is None:
        return 0
    if T.E == b:
        return 1 + (T.Esq.Q if T.Esq else 0)
    elif T.E > b:
        return _count_le(T.Esq, b)
    else:
        q_esq = T.Esq.Q if T.Esq else 0
        return 1 + q_esq + _count_le(T.Dir, b)

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        n = int(tokens[i]); i += 1
        T = None
        a = 0
        b = 0
        if n > 0:
            # teste manual
            for j in range(n):
                x = int(tokens[i]); i += 1
                T = Insere(T, x)
            a = int(tokens[i]); i += 1
            b = int(tokens[i]); i += 1
        else:
            # teste automatico
            T = InsereBal(T, 1, 500000)
            a = 122111
            b = 422111
        for _ in range(100000):
            QuantidadeIntervalo(T, a, b)
        print(QuantidadeIntervalo(T, a, b))

if __name__ == "__main__":
    main()
