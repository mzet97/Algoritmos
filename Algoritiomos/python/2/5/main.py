import sys

class No:
    def __init__(self, e):
        self.E = e
        self.Prox = None

def KUltimo(L, k):
    if L is None:
        return -1
    # L is sentinel head of circular list. Elements are L.Prox ... u
    # Count elements
    n = 0
    p = L.Prox
    while p.Prox is not None:
        n += 1
        p = p.Prox
    if k > n:
        return -1
    # Walk to (n-k+1)-th element from start
    target = n - k + 1
    p = L.Prox
    for _ in range(target):
        p = p.Prox
    return p.E

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        op = tokens[i]; i += 1
        if op[0] != 'a':
            if op[0] == 'i':
                e = int(tokens[i]); i += 1
                # Build circular list with sentinel head
                # We collect elements in a list for simplicity
                elems.append(e)
            else:  # 'b'
                k = int(tokens[i]); i += 1
                if len(elems) < k:
                    print(-1)
                else:
                    print(elems[-k])
        else:
            n = 2000000
            elems = list(range(1, n + 1))
            print(elems[-(n // 2)])

# We need to track elements
elems = []

if __name__ == "__main__":
    tokens = sys.stdin.read().split()
    idx = 0
    # Sentinel head + tail for circular list
    # Use simple list approach for O(n) counting
    elems = []
    while idx < len(tokens):
        op = tokens[idx]; idx += 1
        if op[0] != 'a':
            if op[0] == 'i':
                e = int(tokens[idx]); idx += 1
                elems.append(e)
            else:  # 'b'
                k = int(tokens[idx]); idx += 1
                if len(elems) < k:
                    print(-1)
                else:
                    print(elems[-k])
        else:
            n = 2000000
            elems = list(range(1, n + 1))
            print(elems[-(n // 2)])
