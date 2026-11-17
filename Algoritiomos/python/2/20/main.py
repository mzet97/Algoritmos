import sys

def Intersecao(L1E, L1N, L2E, L2N):
    i = 0; j = 0
    result = []
    while i < L1N and j < L2N:
        if L1E[i] == L2E[j]:
            result.append(L1E[i])
            i += 1
            j += 1
        elif L1E[i] < L2E[j]:
            i += 1
        else:
            j += 1
    return result

def Escreve(L):
    L.sort()
    sys.stdout.write("[")
    for v in L:
        sys.stdout.write(str(v) + " ")
    print("]")

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    while idx < len(tokens):
        n1 = int(tokens[idx]); idx += 1
        n2 = int(tokens[idx]); idx += 1
        if n1 != -1:
            L1 = []
            for i in range(n1):
                L1.append(int(tokens[idx])); idx += 1
            L2 = []
            for i in range(n2):
                L2.append(int(tokens[idx])); idx += 1
            r = Intersecao(L1, n1, L2, n2)
            Escreve(r)
        else:
            n1 = n2 = 9000
            L1 = [i * 2 for i in range(n1)]
            L2 = [i * 3 for i in range(n2)]
            r = Intersecao(L1, n1, L2, n2)
            Escreve(r)

if __name__ == "__main__":
    main()
