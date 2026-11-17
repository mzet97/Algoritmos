import sys

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def Soma2a2(L):
    elems = []
    p = L
    while p is not None:
        elems.append(p.E)
        p = p.Prox
    result_vals = []
    i = 0
    while i + 1 < len(elems):
        result_vals.append(elems[i] + elems[i + 1])
        i += 2
    # Build result list
    result = None
    for v in reversed(result_vals):
        novo = No(v)
        novo.Prox = result
        result = novo
    return result

def Insere(L, e):
    novo = No(e)
    novo.Prox = L
    return novo

def Escreve(L):
    elems = []
    p = L
    while p is not None:
        elems.append(p.E)
        p = p.Prox
    elems.sort()
    print("[ " + " ".join(str(x) for x in elems) + " ]")

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    while idx < len(tokens):
        n = int(tokens[idx]); idx += 1
        L = None
        if n != -1:
            for i in range(n):
                e = int(tokens[idx]); idx += 1
                L = Insere(L, e)
            Lr = Soma2a2(L)
            Escreve(Lr)
        else:
            n = 1000
            for i in range(n):
                L = Insere(L, i)
            Lr = Soma2a2(L)
            Escreve(Lr)

if __name__ == "__main__":
    main()
