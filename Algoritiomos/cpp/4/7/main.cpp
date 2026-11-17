#include <cstdio>
#include <cstdlib>

int Divisores(int n, int k) {
	if (k == 0) return 0;
	int count = Divisores(n, k-1);
	if (n % k == 0) count++;
	return count;
}

int main() {
	setbuf(stdout, NULL); setbuf(stderr, NULL);
	int n; int k;
	while (scanf("%d %d", &n, &k)>0) {
		if (n>=0) {
			/* teste manual */
			printf("%d\n", Divisores(n,k));
		} else {
			/* teste automático */
			n = 50000; k = 50000;
			for (int t=1; t<=200; t++) Divisores(n,k);
			printf("%d\n", Divisores(n,k));
		}
	}

	return 0;
}
