import sys

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def Inverte(L):
    prev = None
    curr = L
    while curr is not None:
        nxt = curr.Prox
        curr.Prox = prev
        prev = curr
        curr = nxt
    return prev

def Insere(L, e):
    novo = No(e)
    novo.Prox = L
    return novo

def Escreve(L):
    p = L
    sys.stdout.write("[ ")
    while p is not None:
        sys.stdout.write(str(p.E) + " ")
        p = p.Prox
    print("]")

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
            Lr = Inverte(L)
            Escreve(Lr)
        else:
            n = 500000
            for i in range(n):
                L = Insere(L, i % 100)
            Lr = Inverte(L)
            Escreve(Lr)

if __name__ == "__main__":
    main()
