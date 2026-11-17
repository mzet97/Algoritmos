import sys

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def Ultimo(L):
    if L is None:
        return -1
    p = L
    while p.Prox is not None:
        p = p.Prox
    return p.E

def Insere(L, u, e):
    novo = No(e)
    if L is None:
        novo.Prox = novo
        L = novo
    else:
        novo.Prox = L
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
        if op != 'a':
            if op == 'i':
                e = int(tokens[i]); i += 1
                L, u = Insere(L, u, e)
            else:  # 'u'
                print(Ultimo(L))
        else:
            for j in range(1, 2000001):
                L, u = Insere(L, u, j)
            print(Ultimo(L))

if __name__ == "__main__":
    main()
