import sys

sys.setrecursionlimit(200000)

def NumeroCaminhos(n, m, X, Y):
    # Build adjacency info: blocked edges
    # Grid is n x n. We need number of paths from (1,1) to (n,n)
    # moving only right or down (since we need shortest paths)
    # Actually, looking at the C++ code, X and Y define forbidden edges
    # or the graph is a DAG with edges X[i]->Y[i]
    # Let's use memoization on a graph

    # Build adjacency list (reverse: which nodes can reach node v)
    # Node numbering: we can think of it as a general graph
    # Actually, the function counts paths from node 1 to node n
    # where edges go from X[i] to Y[i]

    # Build reverse adjacency: who can reach me
    rev = [[] for _ in range(n + 1)]
    for idx in range(m):
        if X[idx] != Y[idx]:
            rev[Y[idx]].append(X[idx])

    memo = {}
    def count(v):
        if v == 1:
            return 1
        if v in memo:
            return memo[v]
        total = 0
        for prev in rev[v]:
            total += count(prev)
        memo[v] = total
        return total

    return count(n)

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        n = int(tokens[i]); i += 1
        m = int(tokens[i]); i += 1
        if n >= 0:
            # teste manual
            X = []
            Y = []
            for j in range(m):
                X.append(int(tokens[i])); i += 1
            for j in range(m):
                Y.append(int(tokens[i])); i += 1
            print(NumeroCaminhos(n, m, X, Y))
        else:
            # teste automatico
            n = 2000; m = 2 * n
            X = []
            Y = []
            for j in range(2, n + 1):
                X.append(j); Y.append(j - 1)
                X.append(j - 1); Y.append(j + 1)
            print(NumeroCaminhos(n, m, X, Y))

if __name__ == "__main__":
    main()
