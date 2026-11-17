#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void PermutacaoCircular(char A[], int n, int k) {
	/* insert your code here */
}

int main() {
	int n,k; 
	char * A; 
	while (scanf("%d %d", &n, &k)>0) {
		A = (char *) malloc(sizeof(char)*(n+600000));
		if (n > 0) {
			char fmt[32];
			snprintf(fmt, sizeof(fmt), "%%%ds", n + 599999);
			scanf(fmt, A);
		} else {
			A[0] = '\0';
		}
		PermutacaoCircular(A, n, k);
		printf("%s\n", A);
		free(A);
	}
}
