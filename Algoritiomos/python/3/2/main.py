import sys
from collections import deque

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    out = []
    while idx < len(tokens):
        n = int(tokens[idx]); idx += 1
        if n > -1:
            nums = [int(tokens[idx + j]) for j in range(n)]
            idx += n
            alternar(nums, n, out)
        else:
            if n == -1:
                n = 100000
                s = 1
                nums = [s * (j + 1) for j in range(n)]
                for j in range(n):
                    nums[j] = (1 if j % 2 == 0 else -1) * (j + 1)
                alternar(nums, n, out)
            elif n == -2:
                n = 100000
                nums = [0] * n
                for j in range(n // 2):
                    nums[j] = j + 1
                for j in range(n // 2, n):
                    nums[j] = -(j + 1)
                alternar(nums, n, out)
            else:
                n = 100000
                nums = [0] * n
                for j in range(n // 2):
                    nums[j] = -(j + 1)
                for j in range(n // 2, n):
                    nums[j] = j + 1
                alternar(nums, n, out)
    sys.stdout.write('\n'.join(out))

def alternar(nums, n, out):
    pos = deque()
    neg = deque()
    for x in nums:
        if x >= 0:
            pos.append(x)
        else:
            neg.append(x)
    line = []
    while pos or neg:
        if pos:
            line.append(str(pos.popleft()))
        if neg:
            line.append(str(neg.popleft()))
    out.append(' '.join(line) + ' ')

if __name__ == "__main__":
    main()
