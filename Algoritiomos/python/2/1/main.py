import sys

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def NumeroElementos(L):
    n = 0
    p = L
    while p is not None:
        n += 1
        p = p.Prox
    return n

def main():
    L = None
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        op = tokens[i]; i += 1
        if op[0] != 'a':
            if op[0] == 'i':
                e = int(tokens[i]); i += 1
                novo = No(e)
                novo.Prox = L
                L = novo
            else:
                print(NumeroElementos(L))
        else:
            for j in range(1, 1000001):
                novo = No(j)
                novo.Prox = L
                L = novo
            print(NumeroElementos(L))

if __name__ == "__main__":
    main()
