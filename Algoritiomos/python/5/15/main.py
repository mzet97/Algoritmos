import sys

sys.setrecursionlimit(200000)

class No:
    def __init__(self, e):
        self.E = e
        self.Esq = None
        self.Dir = None

def Ordenar(A, N):
    # Tree sort: build BST from array, then traverse in-order
    if N <= 1:
        return
    # Build BST
    root = None
    for i in range(N):
        root = _insere_bst(root, A[i])
    # In-order traversal to fill array
    idx = [0]
    _inorder(root, A, idx)

def _insere_bst(T, x):
    if T is None:
        return No(x)
    if x < T.E:
        T.Esq = _insere_bst(T.Esq, x)
    else:
        T.Dir = _insere_bst(T.Dir, x)
    return T

def _inorder(T, A, idx):
    if T is not None:
        _inorder(T.Esq, A, idx)
        A[idx[0]] = T.E
        idx[0] += 1
        _inorder(T.Dir, A, idx)

def InsereBal(e, idx, ini, fim):
    if ini <= fim:
        m = (ini + fim) // 2
        e[idx[0]] = m
        idx[0] += 1
        InsereBal(e, idx, ini, m - 1)
        InsereBal(e, idx, m + 1, fim)

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        val = int(tokens[i]); i += 1
        e = []
        n = 0
        if val > -1:
            # teste manual: preordem de BST
            e = [val]
            while e[-1] != -1:
                e.append(int(tokens[i])); i += 1
            e = e[:-1]
            n = len(e)
        else:
            # teste automatico
            e = [0] * 60000
            idx = [0]
            InsereBal(e, idx, 1, 60000)
            n = idx[0]
        Ordenar(e, n)
        for j in range(n):
            sys.stdout.write(str(e[j]) + " ")
        print()

if __name__ == "__main__":
    main()
