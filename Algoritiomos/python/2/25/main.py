import sys

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def AvaliaPolinomio(P, x):
    # Uses Horner's method
    if P is None:
        return 0
    result = P.E
    p = P.Prox
    while p is not None:
        result = result * x + p.E
        p = p.Prox
    return result

def InsereFinal(L, e, u):
    novo = No(e)
    novo.Prox = None
    if u is None:
        L = novo
    else:
        u.Prox = novo
    u = novo
    return L, u

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
            x = int(tokens[idx]); idx += 1
        else:
            n = 10000000
            for i in range(n + 1):
                P, u = InsereFinal(P, 1, u)
            x = 1
        r = AvaliaPolinomio(P, x)
        print(r)

if __name__ == "__main__":
    main()
