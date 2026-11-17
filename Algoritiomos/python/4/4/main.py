import sys

sys.setrecursionlimit(100000)

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def NumeroElementos(L):
    if L is None:
        return 0
    return 1 + NumeroElementos(L.Prox)

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
            # teste manual: i e = insere; n = calcula numero de elementos
            if op[0] == 'i':
                e = int(tokens[i]); i += 1
                L = Insere(L, e)
            else:
                print(NumeroElementos(L))
        else:
            # teste automatico
            for j in range(1, 50001):
                L = Insere(L, j)
            for _ in range(100):
                NumeroElementos(L)
            print(NumeroElementos(L))

if __name__ == "__main__":
    main()
