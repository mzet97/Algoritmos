import sys

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    out = []
    while idx < len(tokens):
        n = int(tokens[idx]); idx += 1
        if n != -1:
            p1 = []
            for j in range(n):
                e = int(tokens[idx]); idx += 1
                p1.append(e)
            m = int(tokens[idx]); idx += 1
            p2 = []
            for j in range(m):
                e = int(tokens[idx]); idx += 1
                p2.append(e)
        else:
            n = 1000000
            p1 = list(range(1, n + 1))
            p2 = list(range(1, n + 1))
        # Stacks: top is last element. Two stacks identical if same elements in same order.
        out.append('1' if p1 == p2 else '0')
    sys.stdout.write('\n'.join(out))

if __name__ == "__main__":
    main()
