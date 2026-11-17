import sys

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def RemoveMaiores(L, k):
    # Remove elements > k from unsorted linked list
    while L is not None and L.E > k:
        L = L.Prox
    if L is None:
        return None
    prev = L
    p = L.Prox
    while p is not None:
        if p.E > k:
            prev.Prox = p.Prox
        else:
            prev = p
        p = p.Prox
    return L

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
        max_n = int(tokens[idx]); idx += 1
        L = None
        if max_n != -1:
            for i in range(max_n):
                e = int(tokens[idx]); idx += 1
                L = Insere(L, e)
            k = int(tokens[idx]); idx += 1
            L = RemoveMaiores(L, k)
            Escreve(L)
        else:
            max_n = 1000000
            for i in range(max_n):
                L = Insere(L, 2 if i % 2 == 0 else 3)
            L = RemoveMaiores(L, 2)
            Escreve(L)

if __name__ == "__main__":
    main()
