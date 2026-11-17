import sys

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def Concatenar(L1, L2):
    if L1 is None:
        return L2
    if L2 is None:
        return L1
    # Find last node of L1
    p = L1
    while p.Prox is not None:
        p = p.Prox
    p.Prox = L2
    return L1

def Insere(L, e, u):
    novo = No(e)
    novo.Prox = None
    if u is None:
        L = novo
    else:
        u.Prox = novo
    u = novo
    return L, u

def Escreve(L):
    sys.stdout.write("[")
    while L is not None:
        sys.stdout.write(str(L.E) + " ")
        L = L.Prox
    print("]")

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    while idx < len(tokens):
        n1 = int(tokens[idx]); idx += 1
        n2 = int(tokens[idx]); idx += 1
        if n1 != -1:
            L1 = None; u1 = None
            for i in range(n1):
                e = int(tokens[idx]); idx += 1
                L1, u1 = Insere(L1, e, u1)
            L2 = None; u2 = None
            for i in range(n2):
                e = int(tokens[idx]); idx += 1
                L2, u2 = Insere(L2, e, u2)
            L = Concatenar(L1, L2)
            Escreve(L)
        else:
            n1 = n2 = 500000
            L1 = None; u1 = None
            L2 = None; u2 = None
            for i in range(n1):
                e = int(tokens[idx]); idx += 1
                L1, u1 = Insere(L1, e, u1)
            for i in range(n2):
                e = int(tokens[idx]); idx += 1
                L2, u2 = Insere(L2, e, u2)
            L = Concatenar(L1, L2)
            Escreve(L)

if __name__ == "__main__":
    main()
