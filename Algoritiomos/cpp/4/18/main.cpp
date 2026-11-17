#include <cstdio>
#include <cstdlib>
#include <algorithm>

bool DEBUG;

int Maximo(int A[], int n) {
	if (n == 1) return A[0];
	int m = Maximo(A, n-1);
	if (A[n-1] > m) return A[n-1];
	return m;
}

int main(int argc, char * argv[]) {
	setbuf(stdout, NULL); setbuf(stderr, NULL); DEBUG = (argc > 1);
	int n; int * A;
	while (scanf("%d", &n)>0) {
		if (n>=0) {
			/* teste manual */
			A = (int *) malloc(sizeof(int)*n);
			for (int i=0; i<n; i++) scanf("%d", &A[i]);
			printf("%d\n", Maximo(A,n));
			free(A);
		} else {
			/* teste automático */
			n = 50000;
			A = (int *) malloc(sizeof(int)*n);
			for (int i=0; i<n/2; i++) A[i]=i;
			for (int i=n/2+1; i<n; i++) A[i]=n-i;
			for (int i=0; i<20; i++) Maximo(A,n);
			printf("%d\n", Maximo(A,n));
			free(A);
		}
	}

	return 0;
}
