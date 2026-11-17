import sys

def main():
    tokens = sys.stdin.read().split()
    i = 0
    # Two stacks represented as lists
    P1 = []
    P2 = []
    while i < len(tokens):
        op = tokens[i]; i += 1
        if op == 'i':
            e = int(tokens[i]); i += 1
            P1.append(e)
        elif op == 'r':
            # Dequeue: if P2 is empty, move all from P1 to P2
            if not P2:
                while P1:
                    P2.append(P1.pop())
            print(P2.pop())
        elif op == 'p':
            # Peek: if P2 is empty, move all from P1 to P2
            if not P2:
                while P1:
                    P2.append(P1.pop())
            print(P2[-1])
        elif op == 'n':
            print(len(P1) + len(P2))
        else:
            # automatic test
            for j in range(1, 1001):
                P1.append(j)
            total = len(P1) + len(P2)
            while len(P1) + len(P2) > 0:
                print(len(P1) + len(P2), end=' ')
                if not P2:
                    while P1:
                        P2.append(P1.pop())
                print(P2[-1], end=' ')
                print(P2.pop(), end=' ')
            print()

if __name__ == "__main__":
    main()
