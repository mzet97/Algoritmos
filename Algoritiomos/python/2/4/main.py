import sys

class No:
    def __init__(self, e):
        self.E = e
        self.Ant = None
        self.Prox = None

def KUltimo(L, k):
    if L is None:
        return -1
    # Count elements
    n = 1
    p = L
    while p.Prox is not None and p.Prox != L:
        n += 1
        p = p.Prox
    if k > n:
        return -1
    # Walk backward from last node k-1 steps
    p = L
    while p.Prox is not None and p.Prox != L:
        p = p.Prox
    # p is last node
    for _ in range(k - 1):
        p = p.Ant
        if p is None:
            return -1
    return p.E

def Insere(L, e):
    novo = No(e)
    if L is None:
        novo.Prox = novo
        novo.Ant = novo
        L = novo
    else:
        novo.Ant = L.Ant
        L.Ant.Prox = novo
        novo.Prox = L
        L.Ant = novo
        L = novo
    return L

def main():
    L = None
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        op = tokens[i]; i += 1
        if op[0] != 'a':
            if op[0] == 'i':
                e = int(tokens[i]); i += 1
                L = Insere(L, e)
            else:  # 'b'
                k = int(tokens[i]); i += 1
                print(KUltimo(L, k))
        else:
            n = 2000000
            for j in range(1, n + 1):
                L = Insere(L, j)
            if len(op) > 1 and op[1] == '1':
                print(KUltimo(L, n // 2))
            else:
                print(KUltimo(L, 3))
                for _ in range(1000):
                    KUltimo(L, 3)

if __name__ == "__main__":
    main()
