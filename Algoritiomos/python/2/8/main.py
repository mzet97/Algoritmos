import sys

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def SemRepeticoes(L):
    seen = set()
    result = None
    tail = None
    p = L
    while p is not None:
        if p.E not in seen:
            seen.add(p.E)
            novo = No(p.E)
            if result is None:
                result = novo
                tail = novo
            else:
                tail.Prox = novo
                tail = novo
        p = p.Prox
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
    print("[" + "".join(str(x) for x in elems) + "]")

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    while idx < len(tokens):
        n = int(tokens[idx]); idx += 1
        L = None
        if n > -1:
            for i in range(n):
                e = int(tokens[idx]); idx += 1
                L = Insere(L, e)
            Lr = SemRepeticoes(L)
            Escreve(Lr)
        else:
            nlin = n
            n = 1000
            for i in range(n):
                L = Insere(L, 2 if i % 2 == 0 else 3)
            if nlin != -1:
                for i in range(n):
                    L = Insere(L, i)
            Lr = SemRepeticoes(L)
            Escreve(Lr)

if __name__ == "__main__":
    main()
