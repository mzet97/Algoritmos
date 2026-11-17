import sys

def NumeroMaisFrequente(L, n):
    # Find the most frequent number in L using a hash map
    freq = {}
    for i in range(n):
        freq[L[i]] = freq.get(L[i], 0) + 1
    # Find element with max frequency
    best = L[0]
    best_count = 0
    for key, count in freq.items():
        if count > best_count or (count == best_count and key < best):
            best = key
            best_count = count
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
            max_n = 3000000
            L = [1000000 * j for j in range(max_n, 0, -1)]
            L[100000] = 1000000 * 191191
        print(NumeroMaisFrequente(L, max_n))

if __name__ == "__main__":
    main()
