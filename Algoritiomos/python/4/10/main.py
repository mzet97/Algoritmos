import sys

sys.setrecursionlimit(100000)

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def Maior(L):
    if L.Prox is None:
        return L.E
    m = Maior(L.Prox)
    if L.E > m:
        return L.E
    return m

def Insere(L, e):
    novo = No(e)
    novo.Prox = L
    return novo

def main():
    L = None
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        op = tokens[i]; i += 1
        if op[0] != 'a':
            # teste manual: i e = insere; m = busca maior
            if op[0] == 'i':
                e = int(tokens[i]); i += 1
                L = Insere(L, e)
            else:
                print(Maior(L))
        else:
            # teste automatico
            n = 50000
            for j in range(n // 2, n + 1):
                L = Insere(L, j)
            for j in range(1, n // 2):
                L = Insere(L, j)
            for _ in range(100):
                Maior(L)
            print(Maior(L))

if __name__ == "__main__":
    main()
