import sys
import bisect

def ProcuraSoma(L, n, k):
    # Check if there exist i,j such that L[i]+L[j] == k
    L_sorted = sorted(L[:n])
    for i in range(n):
        target = k - L_sorted[i]
        pos = bisect.bisect_left(L_sorted, target)
        if pos < n and L_sorted[pos] == target:
            # Make sure it's not the same element
            if pos != i:
                return 1
            if pos + 1 < n and L_sorted[pos + 1] == target:
                return 1
    return 0

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
            L = list(range(max_n))
            L[200000] = 2000000
            k = 2600000
        print(ProcuraSoma(L, max_n, k))

if __name__ == "__main__":
    main()
