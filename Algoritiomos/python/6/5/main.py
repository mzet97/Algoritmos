import sys

def Distintos(L, N):
    # Count distinct elements using sorting
    if N == 0:
        return 0
    L_sorted = sorted(L[:N])
    count = 1
    for i in range(1, N):
        if L_sorted[i] != L_sorted[i - 1]:
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
        else:
            # automatic test
            max_n = 1000000
            L = [j % 997 for j in range(max_n)]
        print(Distintos(L, max_n))

if __name__ == "__main__":
    main()
