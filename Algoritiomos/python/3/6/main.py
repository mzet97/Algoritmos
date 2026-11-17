import sys

def main():
    tokens = sys.stdin.read().split()
    idx = 0
    out = []
    while idx < len(tokens):
        n = int(tokens[idx]); idx += 1
        if n != -1:
            V = [int(tokens[idx + j]) for j in range(n)]
            idx += n
        else:
            n = 1000
            V = [0] * n
            M = n; m = 1
            for j in range(n - 1, -1, -2):
                V[j] = M; V[j - 1] = m
                M -= 1; m += 1
        manobrar(n, V, out)
    sys.stdout.write('\n'.join(out))

def manobrar(n, V, out):
    # Input track has wagons n, n-1, ..., 2, 1 (1 is first/closest to switch)
    # V[0..n-1] is desired output: V[0] is last wagon, V[n-1] is first
    # We want output sequence to be V[0], V[1], ..., V[n-1]
    # From output side: first out is V[n-1] (the "first" wagon), last out is V[0]
    # Actually looking at the C++ code, the main loop processes V in order.
    # The entry track has wagons: 1 is closest to switch, n is farthest.
    # E=>M: move wagon from entry to maneuver (stack)
    # M=>E: move wagon from maneuver back to entry (not useful typically)
    # M=>S: move wagon from maneuver to output
    # The desired output order (from output side) is V[n-1], V[n-2], ..., V[0]
    # Because in the example: 3,2,1 -> 2,1,3 and V = [2,1,3] (V[0]=2 is last, V[2]=3 is first)
    # Output track: first out is V[n-1]=3? No...

    # Let me reconsider. Entry has n,n-1,...,2,1 where n=last, 1=first.
    # The "first" wagon (1) is closest to the switch/diversion.
    # E=>M takes the first (closest) wagon from entry to maneuver.
    # M=>S takes from maneuver to output (exit track).
    #
    # Desired permutation V[0..n-1]: V[0] is "last" wagon, V[n-1] is "first" wagon
    # on the output track.
    # From the example: n=3, V=[2,1,3]. Entry: 3,2,1 (3=far, 1=near switch).
    # Output should have 2,1,3 meaning 2 is farthest from output end, 3 is closest.
    # So the first wagon to leave output is 3 (V[n-1]).
    #
    # Actually the problem says the output sequence should be V[0] as last, V[n-1] as first.
    # So we process from the input side: we need to produce V[n-1] first, then V[n-2], etc.

    # Entry track is a queue-like: items come off in order 1, 2, ..., n
    # (1 is first/closest to switch)
    # Maneuver track is a stack.
    # Output track: items pushed on, final sequence is V[0] last, V[n-1] first.

    # We want output (from output side, first to last) = V[n-1], V[n-2], ..., V[0]
    # Equivalently: we need to output V_vi = n-1-i for i in range(n)

    moves = []
    entry = list(range(1, n + 1))  # 1 is first (closest to switch)
    entry.reverse()  # entry[0] is closest to switch = 1, entry[-1] = n
    # Actually, entry is a queue: pop from front (closest to switch)
    entry = list(range(1, n + 1))  # index 0 = closest = 1
    stack = []  # maneuver area (stack)
    vi = n - 1  # index into V for next desired output (from V[n-1] down to V[0])

    while vi >= 0:
        target = V[vi]
        # Try to get 'target' either from entry or from stack top
        if stack and stack[-1] == target:
            stack.pop()
            moves.append("M=>S")
            vi -= 1
        elif entry:
            # Move from entry to maneuver
            stack.append(entry.pop(0))
            moves.append("E=>M")
        elif stack and stack[-1] != target:
            # Need to move from maneuver back to entry
            entry.insert(0, stack.pop())
            moves.append("M=>E")
    out.append(','.join(moves))

if __name__ == "__main__":
    main()
