import sys

def ParesIguais(A, B, n):
    i = 0
    j = 0
    count = 0
    while i < n and j < n:
        if A[i] == B[j]:
            # Count duplicates in A
            ca = 0
            val = A[i]
            while i < n and A[i] == val:
                ca += 1
                i += 1
            # Count duplicates in B
            cb = 0
            while j < n and B[j] == val:
                cb += 1
                j += 1
            count += ca * cb
        elif A[i] < B[j]:
            i += 1
        else:
            j += 1
    return count

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    while idx < len(tokens):
        max_n = int(tokens[idx]); idx += 1
        if max_n > -1:
            A = []
            B = []
            for i in range(max_n):
                A.append(int(tokens[idx])); idx += 1
            for i in range(max_n):
                B.append(int(tokens[idx])); idx += 1
            print(ParesIguais(A, B, max_n))
        else:
            nlin = max_n
            max_n = 200000
            A = []
            B = []
            if nlin == -1:
                for i in range(max_n):
                    A.append(i)
                    B.append(i)
            elif nlin == -2:
                for i in range(max_n):
                    A.append(1)
                    B.append(1)
            else:
                for i in range(max_n // 10000):
                    for j in range(10000):
                        A.append(i)
                        B.append(i)
            print(ParesIguais(A, B, max_n))

if __name__ == "__main__":
    main()
