import sys

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def ProdutoPolinomioEscalar(P, a):
    if P is None:
        return None
    head = None; tail = None
    p = P
    while p is not None:
        val = p.E * a
        novo = No(val)
        if head is None:
            head = novo; tail = novo
        else:
            tail.Prox = novo; tail = novo
        p = p.Prox
    return head

def InsereFinal(L, e, u):
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
        n = int(tokens[idx]); idx += 1
        P = None; u = None
        if n != -1:
            for i in range(n + 1):
                e = int(tokens[idx]); idx += 1
                P, u = InsereFinal(P, e, u)
            a = int(tokens[idx]); idx += 1
        else:
            n = 500000
            for i in range(n + 1):
                P, u = InsereFinal(P, 1, u)
            a = 2
        R = ProdutoPolinomioEscalar(P, a)
        Escreve(R)

if __name__ == "__main__":
    main()
