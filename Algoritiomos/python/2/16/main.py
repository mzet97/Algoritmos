import sys

def QuantosContidos(LS, N, LE):
    seen = set()
    p = LE
    while p is not None:
        if p.E in seen:
            seen.add(p.E)
        p = p.Prox
    return len(seen)

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    while idx < len(tokens):
        n = int(tokens[idx]); idx += 1
        if n > -1:
            LS = []
            for i in range(n):
                LS.append(int(tokens[idx])); idx += 1
            nLE = n * n
            LE = None
            for i in range(nLE):
                novo = No(int(tokens[idx])); idx += 1
                novo.Prox = LE
                LE = novo
            r = QuantosContidos(LS, N, LE)
            print(r)
        else:
            n = 3000
            LS = [1] * n
            LE = None
            for i in range(nLE):
                novo = No(2)
                novo.Prox = LE
                LE = novo
            r = QuantosContidos(LS, n, LE)
            print(r)

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

if __name__ == "__main__":
    main()
