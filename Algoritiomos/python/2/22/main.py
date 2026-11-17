import sys

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def InsereBuscaRemove(L, x):
    # L has head sentinel and tail sentinel (value 3000)
    # Search for x between sentinels
    prev = L
    p = L.Prox
    while p.Prox is not None:  # stop before tail sentinel
        if p.E == x:
            # Found: remove it
            prev.Prox = p.Prox
            return True
        prev = p
        p = p.Prox
    # Not found: insert before tail sentinel
    novo = No(x)
    novo.Prox = prev.Prox
    prev.Prox = novo
    return False

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
    p = L.Prox  # skip head sentinel
    sys.stdout.write("[")
    while p.Prox is not None:  # stop before tail sentinel
        sys.stdout.write(str(p.E) + " ")
        p = p.Prox
    print("]")

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    while idx < len(tokens):
        n = int(tokens[idx]); idx += 1
        L = None; u = None
        if n != -1:
            L, u = InsereFinal(L, 3000, u)  # head sentinel
            for i in range(n):
                x = int(tokens[idx]); idx += 1
                L, u = InsereFinal(L, x, u)
            L, u = InsereFinal(L, 3000, u)  # tail sentinel
            x = int(tokens[idx]); idx += 1
            r = InsereBuscaRemove(L, x)
            sys.stdout.write(str(1 if r else 0) + " ")
            Escreve(L)
        else:
            n = 500000
            L, u = InsereFinal(L, 3000, u)
            for i in range(n):
                L, u = InsereFinal(L, i, u)
            L, u = InsereFinal(L, 3000, u)
            r = InsereBuscaRemove(L, 300000)
            sys.stdout.write(str(1 if r else 0) + " ")
            Escreve(L)

if __name__ == "__main__":
    main()
