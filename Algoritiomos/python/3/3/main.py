import sys

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    out = []
    while idx < len(tokens):
        n = int(tokens[idx]); idx += 1
        P = []
        if n >= 0:
            for j in range(n):
                e = int(tokens[idx]); idx += 1
                P.append(e)
        else:
            if n == -1:
                for j in range(1, 1001):
                    P.append(j + 1001 * (j % 2))
            else:
                for j in range(1, 1001):
                    P.append(j + 1001 * (1 if j % 5 != 0 else 0))
        # Sort stack so pop order is ascending
        # P is a stack: top = P[-1], pop gives P[-1]
        # Escreve pops and prints: so output is P[-1], P[-2], ...
        # We want output ascending: P[-1] <= P[-2] <= ... => P sorted descending
        P.sort(reverse=True)
        result = []
        while P:
            result.append(str(P.pop()))
        out.append(' '.join(result) + ' ')
    sys.stdout.write('\n'.join(out))

if __name__ == "__main__":
    main()
