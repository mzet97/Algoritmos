import sys

class No:
    def __init__(self, col, E):
        self.col = col
        self.E = E
        self.Prox = None

class MatrizEsparsa:
    def __init__(self, N):
        self.N = N
        self.linhas = [[] for _ in range(N)]

def AoQuadrado(M):
    N = M.N
    R = MatrizEsparsa(N)
    # For each pair (i,k) where M[i][k] != 0 and M[k][j] != 0, add M[i][k]*M[k][j] to R[i][j]
    # Convert M to dict for easy access
    row_dicts = [{} for _ in range(N)]
    p_list = []
    for i in range(N):
        p = M.linhas[i]
        while p is not None:
            row_dicts[i][p.col] = p.E
            p_list.append((i, p.col, p.E))
            p = p.Prox

    # Compute M^2 = M*M
    result = [{} for _ in range(N)]
    for i in range(N):
        for k_val, row_dicts[i]:
            for j_val in row_dicts[k_val]:
                val = row_dicts[i][k_val] * row_dicts[k_val][j_val]
                if j_val in result[i]:
                    result[i][j_val] += val
                else:
                    result[i][j_val] = val

    # Build result as linked lists
    for i in range(N):
        cols = sorted(result[i].keys())
        for c in cols:
            novo = No(c, result[i][c])
            novo.Prox = R.linhas[i]
            R.linhas[i] = novo
    return R

def Escreve(M):
    sys.stdout.write("[")
    for i in range(M.N):
        sys.stdout.write("[")
        p = M.linhas[i]
        while p is not None:
            sys.stdout.write("c=%d;e=%d " % (p.col, p.E))
            p = p.Prox
        sys.stdout.write("]")
    print("]")

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    while idx < len(tokens):
    n = int(tokens[idx]); idx += 1
    k = int(tokens[idx]); idx += 1
    M = MatrizEsparsa(n)
    if n != -1:
        u_list = [None] * n
        ul = 0
        for i in range(k):
            l = int(tokens[idx]); idx += 1
            c = int(tokens[idx]); idx += 1
            e = int(tokens[idx]); idx += 1
            novo = No(c, e)
            if l != ul:
                u_list[l] = None
            if u_list[l] is None:
                M.linhas[l] = novo
            else:
                u_list[l].Prox = novo
            u_list[l] = novo
        M2 = AoQuadrado(M)
        Escreve(M2)
    else:
        M = MatrizEsparsa(1000)
        for l in range(10):
            u = None
            for c in range(10):
                novo = No(c, 1)
                if u is None:
                    M.linhas[l] = novo
                else:
                    u.Prox = novo
                u = novo
        M2 = AoQuadrado(M)
        Escreve(M2)

if __name__ == "__main__":
    main()
