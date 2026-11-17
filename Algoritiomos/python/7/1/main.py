import sys
import heapq

def Ordenar(L, n):
    # HeapSort: use a min-heap
    heapq.heapify(L[:n])  # doesn't work in-place for the list
    # Actually, use heap sort properly
    heap = L[:]
    heapq.heapify(heap)
    for i in range(n):
        L[i] = heapq.heappop(heap)

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        n = int(tokens[i]); i += 1
        if n != -1:
            L = []
            for j in range(n):
                L.append(int(tokens[i])); i += 1
        else:
            # automatic test
            n = 100000
            L = list(range(n, 0, -1))
        Ordenar(L, n)
        for j in range(n):
            sys.stdout.write(str(L[j]) + " ")
        print()

if __name__ == "__main__":
    main()
