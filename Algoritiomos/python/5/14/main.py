import sys

sys.setrecursionlimit(200000)

class No:
    def __init__(self, e):
        self.E = e
        self.Esq = None
        self.Dir = None

def CriarArvoreAlturaMinima(x, ini, fim):
    if ini > fim:
        return None
    m = (ini + fim) // 2
    T = No(x[m])
    T.Esq = CriarArvoreAlturaMinima(x, ini, m - 1)
    T.Dir = CriarArvoreAlturaMinima(x, m + 1, fim)
    return T

def Escrever(T):
    if T is not None:
        sys.stdout.write("(%d " % T.E)
        sys.stdout.write("(")
        Escrever(T.Esq)
        sys.stdout.write(")")
        sys.stdout.write("(")
        Escrever(T.Dir)
        sys.stdout.write("))")

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        val = int(tokens[i]); i += 1
        e = []
        if val > -1:
            # teste manual
            e = [val]
            while e[-1] != -1:
                e.append(int(tokens[i])); i += 1
            e = e[:-1]  # remove the -1 sentinel
        else:
            # teste automatico
            e = list(range(1, 100001))
        T = CriarArvoreAlturaMinima(e, 0, len(e) - 1)
        Escrever(T)
        print()

if __name__ == "__main__":
    main()
