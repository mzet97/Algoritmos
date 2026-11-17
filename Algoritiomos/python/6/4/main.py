import sys

def SomaMaisProximaZero(L, n):
    # Sort, then use two pointers from ends
    L_sorted = sorted(L[:n])
    best = float('inf')
    lo = 0
    hi = n - 1
    while lo < hi:
        s = L_sorted[lo] + L_sorted[hi]
        if abs(s) < abs(best):
            best = s
        if s == 0:
            return 0
        elif s < 0:
            lo += 1
        else:
            hi -= 1
    return best

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        max_n = int(tokens[i]); i += 1
        if max_n != -1:
            L = []
            for j in range(max_n):
                L.append(int(tokens[i])); i += 1
        else:
            # automatic test
            max_n = 100000
            L = list(range(1, max_n + 1))
            L[50000] = -20000
        print(SomaMaisProximaZero(L, max_n))

if __name__ == "__main__":
    main()
