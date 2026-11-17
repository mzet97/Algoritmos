import sys
import bisect

def BuscaEmLote(L, B, n):
    # Sort L, then for each element in B, check if it exists in L
    L_sorted = sorted(L[:n])
    count = 0
    for i in range(n):
        pos = bisect.bisect_left(L_sorted, B[i])
        if pos < n and L_sorted[pos] == B[i]:
            count += 1
    return count

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        max_n = int(tokens[i]); i += 1
        if max_n != -1:
            L = []
            for j in range(max_n):
                L.append(int(tokens[i])); i += 1
            B = []
            for j in range(max_n):
                B.append(int(tokens[i])); i += 1
        else:
            # automatic test
            max_n = 1000000
            L = list(range(max_n))
            B = [2 * j for j in range(max_n)]
        print(BuscaEmLote(L, B, max_n))

if __name__ == "__main__":
    main()
