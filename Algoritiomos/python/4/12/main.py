import sys

sys.setrecursionlimit(100000)

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def Ultimo(L):
    if L.Prox is None:
        return L.E
    return Ultimo(L.Prox)

def Insere(L, u, e):
    novo = No(e)
    if L is None:
        L = novo
    else:
        u.Prox = novo
    u = novo
    return L, u

def main():
    L = None
    u = None
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        op = tokens[i]; i += 1
        if op[0] != 'a':
            # teste manual: i e = insere; u = busca ultimo
            if op[0] == 'i':
                e = int(tokens[i]); i += 1
                L, u = Insere(L, u, e)
            else:
                print(Ultimo(L))
        else:
            # teste automatico
            for j in range(1, 50001):
                L, u = Insere(L, u, j)
            for _ in range(100):
                Ultimo(L)
            print(Ultimo(L))

if __name__ == "__main__":
    main()
