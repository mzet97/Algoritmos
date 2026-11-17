import sys

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    out = []
    while idx < len(tokens):
        n = int(tokens[idx]); idx += 1
        if n != -1:
            S = [int(tokens[idx + j]) for j in range(n)]
            idx += n
        else:
            n = 1000000
            S = []
            for j in range(n // 2):
                S.append(1)
            for j in range(n // 2):
                S.append(0)
        # Compute sum with undo: 0 means undo (pop last), otherwise push
        stack = []
        for x in S:
            if x != 0:
                stack.append(x)
            elif stack:
                stack.pop()
        out.append(str(sum(stack)))
    sys.stdout.write('\n'.join(out))

if __name__ == "__main__":
    main()
