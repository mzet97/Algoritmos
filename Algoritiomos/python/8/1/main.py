import sys

def SaoAnagramas(A, B, n):
    # Count character frequencies
    freq = [0] * 256
    for i in range(n):
        freq[ord(A[i])] += 1
    for i in range(n):
        freq[ord(B[i])] -= 1
    for i in range(256):
        if freq[i] != 0:
            return 0
    return 1

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        max_n = int(tokens[i]); i += 1
        if max_n != -1:
            A = tokens[i]; i += 1
            B = tokens[i]; i += 1
        else:
            # automatic test
            max_n = 1000000
            T = ord('z') - ord('a') + 1
            A = ''.join(chr(ord('a') + (j % T)) for j in range(max_n - 1, -1, -1))
            B = ''.join(chr(ord('a') + (j % T)) for j in range(max_n))
        print(SaoAnagramas(A, B, max_n))

if __name__ == "__main__":
    main()
