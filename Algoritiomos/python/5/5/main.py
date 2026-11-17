import sys

sys.setrecursionlimit(200000)

class No:
    def __init__(self, e):
        self.E = e
        self.Esq = None
        self.Dir = None

FIM_ENTRADA = -1000
ENTRADA_AUTOMATICA = -2000

def MaiorSubarvore(T):
    if T is None:
        return 0
    # MaiorSubarvore: soma da subarvore com maior soma
    def helper(node):
        if node is None:
            return 0, 0  # (soma deste no, melhor soma)
        s_esq, m_esq = helper(node.Esq)
        s_dir, m_dir = helper(node.Dir)
        s = node.E + s_esq + s_dir
        m = max(s, m_esq, m_dir)
        return s, m
    _, result = helper(T)
    return result

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
    T = No(-2 if h > 1 else 1)
    T.Esq = PreencheCheia(h - 1)
    T.Dir = PreencheCheia(h - 1)
    return T

def PreencheZigZag(h):
    if h == 0:
        return None
    T = No(-2 if h > 1 else 1)
    T.Esq = PreencheZigZag(h - 1)
    T.Dir = None
    return T

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
            print(MaiorSubarvore(T))
        else:
            # teste automatico
            if val == ENTRADA_AUTOMATICA:
                T = PreencheZigZag(50000)
            else:
                T = PreencheCheia(19)
            print(MaiorSubarvore(T))

if __name__ == "__main__":
    main()
