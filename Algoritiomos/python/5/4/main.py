import sys

sys.setrecursionlimit(200000)

class No:
    def __init__(self, e):
        self.E = e
        self.Esq = None
        self.Dir = None

FIM_ENTRADA = -1000
ENTRADA_AUTOMATICA = -2000

def ListaNosInternos(T):
    if T is None:
        return None
    if T.Esq is None and T.Dir is None:
        return None
    # T is internal: has at least one child
    cabeca = NoNoL(T.E)
    esq_list = ListaNosInternos(T.Esq)
    dir_list = ListaNosInternos(T.Dir)
    # Concatenate: esq_list + cabeca + dir_list
    result = esq_list
    if result is None:
        result = cabeca
    else:
        p = result
        while p.Prox is not None:
            p = p.Prox
        p.Prox = cabeca
    cabeca.Prox = dir_list
    return result

class NoL:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def NoNoL(e):
    return NoL(e)

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

def PreencheCheia(h):
    if h == 0:
        return None
    T = No(h)
    T.Esq = PreencheCheia(h - 1)
    T.Dir = PreencheCheia(h - 1)
    return T

def PreencheZigZag(h):
    if h == 0:
        return None
    T = No(h)
    T.Esq = PreencheZigZag(h - 1)
    T.Dir = None
    return T

def Escreve(L):
    n = 0
    p = L
    while p is not None:
        n += 1
        p = p.Prox
    Lv = []
    p = L
    while p is not None:
        Lv.append(p.E)
        p = p.Prox
    Lv.sort()
    sys.stdout.write("[ ")
    for v in Lv:
        sys.stdout.write(str(v) + " ")
    sys.stdout.write("]\n")

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        val = int(tokens[i]); i += 1
        T = None
        if val > ENTRADA_AUTOMATICA:
            # teste manual: visita preordem de T (0 para nulo)
            e = [val]
            while e[-1] != FIM_ENTRADA:
                e.append(int(tokens[i])); i += 1
            idx = [0]
            T = Insere(T, e, idx)
        else:
            # teste automatico
            if val == ENTRADA_AUTOMATICA:
                T = PreencheZigZag(50000)
            else:
                T = PreencheCheia(19)
        L = ListaNosInternos(T)
        Escreve(L)

if __name__ == "__main__":
    main()
