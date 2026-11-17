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

def ProdutoPolinomioEscalar(Q, a):
    if Q is None:
        return None
    head = None; tail = None
    q = Q
    while q is not None:
        val = q.E * a
        novo = No(val)
        if head is None:
            head = novo; tail = novo
        else:
            tail.Prox = novo; tail = novo
        q = q.Prox
    return head

def InsereFinalEnc(L, e, u):
    novo = No(e)
    novo.Prox = None
    if u is None:
        L = novo
    else:
        u.Prox = novo
    u = novo
    return L, u

def InsereInicioEnc(L, e):
    novo = No(e)
    novo.Prox = L
    return novo

def Destroi(L):
    while L is not None:
        nxt = L.Prox
        L = nxt

def ProdutoPolinomios(P, Q):
    R = None; uR = None
    R, uR = InsereFinalEnc(R, 0, uR)
    zeros = 0
    p = P
    while p is not None:
        T = ProdutoPolinomioEscalar(Q, p.E)
        for i in range(zeros):
            T = InsereInicioEnc(T, 0)
        Rtemp = SomaPolinomios(R, T)
        Destroi(T)
        Destroi(R)
        R = Rtemp
        zeros += 1
        p = p.Prox
    return R

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
                P, uP = InsereFinalEnc(P, e, uP)
            for i in range(n2 + 1):
                e = int(tokens[idx]); idx += 1
                Q, uQ = InsereFinalEnc(Q, e, uQ)
        else:
            n1 = 1000; n2 = 1000
            for i in range(n1 + 1):
                P, uP = InsereFinalEnc(P, 1, uP)
            for i in range(n2 + 1):
                Q, uQ = InsereFinalEnc(Q, 1, uQ)
        R = ProdutoPolinomios(P, Q)
        Escreve(R)

if __name__ == "__main__":
    main()
