import sys

def RemoveMaiores(L, n, k):
    j = 0
    for i in range(n):
        if L[i] <= k:
            L[j] = L[i]
            j += 1
    return j

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    while idx < len(tokens):
        max_n = int(tokens[idx]); idx += 1
        if max_n != -1:
            L = []
            for i in range(max_n):
                e = int(tokens[idx]); idx += 1
                L.append(e)
            k = int(tokens[idx]); idx += 1
            n = RemoveMaiores(L, max_n, k)
            L_sorted = sorted(L[:n])
            print("[" + "".join(str(x) for x in L_sorted) + "]")
        else:
            max_n = 1000000
            L = []
            for i in range(max_n):
                L.append(2 if i % 2 == 0 else 3)
            n = RemoveMaiores(L, max_n, 2)
            L_sorted = sorted(L[:n])
            print("[" + "".join(str(x) for x in L_sorted) + "]")

if __name__ == "__main__":
    main()
