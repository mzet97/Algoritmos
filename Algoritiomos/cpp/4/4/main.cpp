#include <cstdio>
#include <cstdlib>

long long int Bin(int n, int k) {
	// assume n >= k >= 0
	// retorna binomial de n,k = n! / (k! (n-k)!)
	if (k == 0 || k == n) return 1;
	return Bin(n-1, k-1) + Bin(n-1, k);
}

int main() {
	setbuf(stdout, NULL); setbuf(stderr, NULL);
	int n; int k;
	while (scanf("%d %d", &n, &k)>0) {
		if (n>=0) {
			/* teste manual */
			printf("%lld\n", Bin(n,k));
		} else {
			/* teste automático */
			n = 50000; k = 49999;
			for (int t=1; t<=200; t++) Bin(n,k);
			printf("%lld\n", Bin(n,k));
		}
	}

	return 0;
}
