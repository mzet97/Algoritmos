import sys

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def Maior(L):
    m = L.E
    p = L.Prox
    while p is not None:
        if p.E > m:
            m = p.E
        p = p.Prox
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
        if op != 'a':
            if op == 'i':
                e = int(tokens[i]); i += 1
                L = Insere(L, e)
            else:  # 'm'
                print(Maior(L))
        else:
            n = 1000000
            for j in range(n // 2, n + 1):
                L = Insere(L, j)
            for j in range(1, n // 2):
                L = Insere(L, j)
            print(Maior(L))

if __name__ == "__main__":
    main()
