import sys
import bisect

def QuantidadeIntervalo(L, n, a, b):
    # L is sorted. Count elements in [a, b]
    L_sorted = sorted(L[:n])
    lo = bisect.bisect_left(L_sorted, a)
    hi = bisect.bisect_right(L_sorted, b)
    return hi - lo

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        max_n = int(tokens[i]); i += 1
        a = 0
        b = 0
        if max_n != -1:
            L = []
            for j in range(max_n):
                L.append(int(tokens[i])); i += 1
            a = int(tokens[i]); i += 1
            b = int(tokens[i]); i += 1
        else:
            # automatic test
            max_n = 1000000
            L = [3 * j for j in range(max_n)]
            a = 3 * 200000
            b = 3 * 900000
        for _ in range(100000):
            QuantidadeIntervalo(L, max_n, a, b)
        print(QuantidadeIntervalo(L, max_n, a, b))

if __name__ == "__main__":
    main()
