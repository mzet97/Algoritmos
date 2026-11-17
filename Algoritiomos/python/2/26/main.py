import sys

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def SomaPolinomios(P, Q):
    if P is None and Q is None:
        return None
    head = None; tail = None
    p = P; q = Q
    while p is not None or q is not None:
        if p is not None and q is not None:
            val = p.E + q.E
            p = p.Prox; q = q.Prox
        elif p is not None:
            val = p.E
            p = p.Prox
        else:
            val = q.E
            q = q.Prox
        novo = No(val)
        if head is None:
            head = novo; tail = novo
        else:
            tail.Prox = novo; tail = novo
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
        n1 = int(tokens[idx]); idx += 1
        n2 = int(tokens[idx]); idx += 1
        P = None; uP = None
        Q = None; uQ = None
        if n1 != -1:
            for i in range(n1 + 1):
                e = int(tokens[idx]); idx += 1
                P, uP = InsereFinal(P, e, uP)
            for i in range(n2 + 1):
                e = int(tokens[idx]); idx += 1
                Q, uQ = InsereFinal(Q, e, uQ)
        else:
            n1 = 1000000; n2 = 1000000
            P, uP = InsereFinal(P, 0, uP)
            for i in range(n1):
                P, uP = InsereFinal(P, 1, uP)
            Q, uQ = InsereFinal(Q, 1, uQ)
            for i in range(n2):
                Q, uQ = InsereFinal(Q, -1, uQ)
        R = SomaPolinomios(P, Q)
        Escreve(R)

if __name__ == "__main__":
    main()
