import sys

def ParesForaOrdem(L, n):
    # Count inversions using merge sort
    _, count = _merge_sort_count(L[:n])
    return count

def _merge_sort_count(arr):
    if len(arr) <= 1:
        return arr, 0
    mid = len(arr) // 2
    left, cl = _merge_sort_count(arr[:mid])
    right, cr = _merge_sort_count(arr[mid:])
    merged, cm = _merge_count(left, right)
    return merged, cl + cr + cm

def _merge_count(left, right):
    result = []
    i = 0
    j = 0
    count = 0
    while i < len(left) and j < len(right):
        if left[i] <= right[j]:
            result.append(left[i])
            i += 1
        else:
            result.append(right[j])
            count += len(left) - i
            j += 1
    result.extend(left[i:])
    result.extend(right[j:])
    return result, count

def main():
    tokens = sys.stdin.read().split()
    i = 0
    while i < len(tokens):
        max_n = int(tokens[i]); i += 1
        if max_n != -1:
            L = []
            for j in range(max_n):
                L.append(int(tokens[i])); i += 1
        else:
            # automatic test
            max_n = 50000
            L = list(range(max_n, 0, -1))
        print(ParesForaOrdem(L, max_n))

if __name__ == "__main__":
    main()
