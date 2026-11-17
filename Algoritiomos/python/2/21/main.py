import sys

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def Diferenca(L1, n1, L2_elems):
    # L1 and L2 are sorted linked lists
    result = []
    p1 = L1
    p2 = L2_elems
    while p1 is not None and p2 is not None:
        if p1.E == p2.E:
            p1 = p1.Prox
            p2 = p2.Prox
        elif p1.E < p2.E:
            result.append(p1.E)
            p1 = p1.Prox
        else:
            p2 = p2.Prox
    while p1 is not None:
        result.append(p1.E)
        p1 = p1.Prox
    return result

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
    elems = []
    p = L
    while p is not None:
        elems.append(p.E)
        p = p.Prox
    elems.sort()
    sys.stdout.write("[")
    for v in elems:
        sys.stdout.write(str(v) + " ")
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
            r = Diferenca(L1, n1, L2)
            Escreve(r)
        else:
            n1 = n2 = 9000
            L1 = None; u1 = None
            L2 = None; u2 = None
            for i in range(n1):
                L1, u1 = Insere(L1, i * 2, u1)
            for i in range(n2):
                L2, u2 = Insere(L2, i * 3, u2)
            r = Diferenca(L1, n1, L2)
            Escreve(r)

if __name__ == "__main__":
    main()
