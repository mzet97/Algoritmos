import sys
from collections import deque

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    out = []
    while idx < len(tokens):
        k = int(tokens[idx]); idx += 1
        # Read numbers until 0
        nums = []
        while idx < len(tokens):
            x = int(tokens[idx]); idx += 1
            if x == 0:
                break
            nums.append(x)
        # Keep only last k elements using a deque of size k
        buf = deque(maxlen=k)
        for x in nums:
            buf.append(x)
        line = ' '.join(str(x) for x in buf) + ' '
        out.append(line)
    sys.stdout.write('\n'.join(out))

if __name__ == "__main__":
    main()
