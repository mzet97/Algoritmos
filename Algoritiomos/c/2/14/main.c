#include <stdio.h>
#include <stdlib.h>

/* Search for x in sorted matrix where rows and columns are sorted.
   O(n log n) approach: binary search each row. */
int Busca(int **B, int n, int x) {
    for (int i = 0; i < n; i++) {
        /* Binary search in row i */
        int lo = 0, hi = n - 1;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            if (B[i][mid] == x) return 1;
            else if (B[i][mid] < x) lo = mid + 1;
            else hi = mid - 1;
        }
    }
    return 0;
}

int C_Busca(int **B, int n, int x, int q) {
#ifndef CODEV
    printf("CODEV_BEGIN_EXEC\n");
#endif
    int r;
    for (int t=0; t<q; t++) {
        r = Busca(B,n,x);
    }
#ifndef CODEV
    printf("CODEV_END_EXEC\n");
#endif
    return r;
}

int main() {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    int n; int ** B;
    while (scanf("%d", &n)>0) {
        if (n>-1) {
            B = (int **) malloc(sizeof(int*)*n);
            for (int i=0; i<n; i++) {
                B[i] = (int *) malloc(sizeof(int)*n);
                for (int j=0; j<n; j++) {
                    scanf("%d", &(B[i][j]));
                }
            }
            int x; scanf("%d", &x);
            printf("%d\n", C_Busca(B,n,x,1));
            for (int i=0; i<n; i++) {
                free(B[i]);
            }
            free(B);
        } else {
            int nlin = n;
            n = 16000; int x;
            B = (int **) malloc(sizeof(int*)*n);
            for (int i=0; i<n; i++) {
                B[i] = (int *) malloc(sizeof(int)*n);
            }
            if ((nlin==-1)|| (nlin==-2)) {
                for (int i=0; i<n; i++) {
                    for (int j=0; j<n; j++) {
                        B[i][j] = i;
                    }
                }
                B[n-1][n-2] = n; B[n-1][n-1] = n+1;
                if (nlin==-1) {
                    printf("%d\n", C_Busca(B,n,n,1));
                } else {
                    printf("%d\n", C_Busca(B,n,n+2,1));
                }
            } else if ((nlin==-3)|| (nlin==-4)) {
                for (int i=0; i<n; i++) {
                    for (int j=0; j<n; j++) {
                        B[i][j] = j;
                    }
                }
                B[n-1][n-2] = n; B[n-1][n-1] = n+1;
                if (nlin==-3) {
                    printf("%d\n", C_Busca(B,n,n,1));
                } else {
                    printf("%d\n", C_Busca(B,n,n+2,1));
                }
            } else {
                for (int i=0; i<n; i++) {
                    for (int j=0; j<n; j++) {
                        B[i][j] = 3;
                    }
                }
                B[0][1] = 2; B[0][0] = 1;
                if (nlin==-5) {
                    printf("%d\n", C_Busca(B,n,2,1));
                } else {
                    printf("%d\n", C_Busca(B,n,0,1));
                    C_Busca(B,n,0,10);
                }
            }
            for (int i=0; i<n; i++) {
                free(B[i]);
            }
            free(B);
        }
    }
    return 0;
}
