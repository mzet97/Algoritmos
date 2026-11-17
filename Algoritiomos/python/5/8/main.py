import sys

sys.setrecursionlimit(200000)

class No:
    def __init__(self, e):
        self.E = e
        self.niv = 0
        self.Esq = None
        self.Dir = None

FIM_ENTRADA = -1000
ENTRADA_AUTOMATICA = -2000

def AtualizaNivel(T, nivel=0):
    if T is None:
        return
    T.niv = nivel
    AtualizaNivel(T.Esq, nivel + 1)
    AtualizaNivel(T.Dir, nivel + 1)

def Insere(T, e, idx):
    i = idx[0]
    if e[i] == 0:
        i += 1
        idx[0] = i
        return None
    T = No(e[i])
    i += 1
    idx[0] = i
    T.Esq = Insere(T.Esq, e, idx)
    T.Dir = Insere(T.Dir, e, idx)
    return T

def PreencheCheia(h, counter):
    if h == 0:
        return None
    T = No(counter[0])
    counter[0] += 1
    T.Esq = PreencheCheia(h - 1, counter)
    T.Dir = PreencheCheia(h - 1, counter)
    return T

def PreencheZigZag(h, counter):
    if h == 0:
        return None
    T = No(counter[0])
    counter[0] += 1
    T.Esq = PreencheZigZag(h - 1, counter)
    T.Dir = None
    return T

def Escreve(T):
    if T is not None:
        sys.stdout.write("niv(%d)=%d " % (T.E, T.niv))
        Escreve(T.Esq)
        Escreve(T.Dir)

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        val = int(tokens[i]); i += 1
        T = None
        if val > ENTRADA_AUTOMATICA:
            # teste manual
            e = [val]
            while e[-1] != FIM_ENTRADA:
                e.append(int(tokens[i])); i += 1
            idx = [0]
            T = Insere(T, e, idx)
        else:
            # teste automatico
            counter = [1]
            if val == ENTRADA_AUTOMATICA:
                T = PreencheCheia(19, counter)
            else:
                T = PreencheZigZag(50000, counter)
        AtualizaNivel(T)
        Escreve(T)
        print()
        T = None

if __name__ == "__main__":
    main()
