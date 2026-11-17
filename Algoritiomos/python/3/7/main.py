import sys

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    out = []
    while idx < len(tokens):
        n = int(tokens[idx]); idx += 1
        if n != -1:
            f1 = []
            for j in range(n):
                e = int(tokens[idx]); idx += 1
                f1.append(e)
            m = int(tokens[idx]); idx += 1
            f2 = []
            for j in range(m):
                e = int(tokens[idx]); idx += 1
                f2.append(e)
        else:
            n = 1000000
            f1 = list(range(1, n + 1))
            f2 = list(range(1, n + 1))
        # Check if identical
        out.append('1' if f1 == f2 else '0')
    sys.stdout.write('\n'.join(out))

if __name__ == "__main__":
    main()
