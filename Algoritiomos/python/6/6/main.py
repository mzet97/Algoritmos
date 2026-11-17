import sys

def MaiorDiferenca(L, n):
    # Find max(L[j] - L[i]) for j > i (max profit problem)
    # This is the maximum difference where larger element comes after smaller
    if n < 2:
        return 0
    min_so_far = L[0]
    max_diff = L[1] - L[0]
    for i in range(1, n):
        diff = L[i] - min_so_far
        if diff > max_diff:
            max_diff = diff
        if L[i] < min_so_far:
            min_so_far = L[i]
    return max_diff

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
            L = list(range(max_n, 0, -1))
            L[500000] = -1
            L[70000] = 3000000
        for _ in range(200):
            MaiorDiferenca(L, max_n)
        print(MaiorDiferenca(L, max_n))

if __name__ == "__main__":
    main()
