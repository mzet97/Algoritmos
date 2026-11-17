import sys

def OrdenacaoPorContagem(L, n):
    if n == 0:
        return
    # Counting sort: find range, count occurrences, rebuild
    min_val = min(L)
    max_val = max(L)
    offset = min_val
    size = max_val - min_val + 1
    count = [0] * size
    for i in range(n):
        count[L[i] - offset] += 1
    idx = 0
    for v in range(size):
        for _ in range(count[v]):
            L[idx] = v + offset
            idx += 1

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
            max_n = 500000
            L = []
            for j in range(max_n, 0, -2):
                L.append(j)
                L.append(j)
        OrdenacaoPorContagem(L, max_n)
        for j in range(max_n):
            sys.stdout.write(str(L[j]) + ",")
        print()

if __name__ == "__main__":
    main()
