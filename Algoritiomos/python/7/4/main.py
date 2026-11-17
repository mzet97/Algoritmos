import sys

def Histograma(X, n, k):
    H = [0] * k
    for i in range(n):
        if 0 <= X[i] < k:
            H[X[i]] += 1
    return H

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        max_n = int(tokens[i]); i += 1
        if max_n != -1:
            L = []
            for j in range(max_n):
                L.append(int(tokens[i])); i += 1
            k = int(tokens[i]); i += 1
        else:
            # automatic test
            max_n = 1000000
            L = list(range(max_n, 0, -1))
            k = 100000
        H = Histograma(L, max_n, k)
        for j in range(k):
            sys.stdout.write("%d: %d; " % (j, H[j]))
        print()

if __name__ == "__main__":
    main()
