import sys
import heapq

def ObterMediana(C):
    return C['med']

def Insere(C, x):
    ante = C['ante']  # max-heap (stored as negated values)
    post = C['post']  # min-heap

    if C['n'] == 0:
        heapq.heappush(ante, -x)
        C['med'] = float(x)
    elif C['n'] % 2 == 1:
        # Currently odd number of elements -> will be even
        if x <= -ante[0]:
            heapq.heappush(ante, -x)
            val = -heapq.heappop(ante)
            heapq.heappush(post, val)
        else:
            heapq.heappush(post, x)
        C['med'] = (-ante[0] + post[0]) / 2.0
    else:
        # Currently even number -> will be odd
        if x <= -ante[0]:
            heapq.heappush(ante, -x)
        else:
            heapq.heappush(post, x)
        # Balance: ante should have one more than post
        if len(ante) > len(post) + 1:
            val = -heapq.heappop(ante)
            heapq.heappush(post, val)
        elif len(post) > len(ante):
            val = -heapq.heappop(ante) if ante else 0
            heapq.heappush(post, val)
            # Actually need to rebalance properly
            val2 = heapq.heappop(post)
            heapq.heappush(ante, -val2)
        C['med'] = float(-ante[0])

    C['n'] += 1

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
            L = list(range(n))
        C = {'ante': [], 'post': [], 'n': 0, 'med': 0.0}
        for j in range(n):
            Insere(C, L[j])
            sys.stdout.write("%.1f " % ObterMediana(C))
        print()

if __name__ == "__main__":
    main()
