import sys
import heapq

def Maiores(L, n, k, M):
    # Find k largest elements using a min-heap of size k
    heap = []
    for i in range(n):
        if len(heap) < k:
            heapq.heappush(heap, L[i])
        elif L[i] > heap[0]:
            heapq.heapreplace(heap, L[i])
    heap.sort()
    for i in range(k):
        M[i] = heap[i]

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        n = int(tokens[i]); i += 1
        if n != -1:
            L = []
            for j in range(n):
                L.append(int(tokens[i])); i += 1
            k = int(tokens[i]); i += 1
        else:
            # automatic test
            n = 2000000
            L = list(range(n))
            k = 100
        M = [0] * k
        Maiores(L, n, k, M)
        for j in range(k):
            sys.stdout.write(str(M[j]) + " ")
        print()

if __name__ == "__main__":
    main()
