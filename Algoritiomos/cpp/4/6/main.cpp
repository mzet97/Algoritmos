#include <cstdio>
#include <cstdlib>

int RaizQuadHelper(int n, int lo, int hi) {
	if (lo == hi) return lo;
	int mid = lo + (hi - lo) / 2;
	if ((long long)mid * mid > n) return RaizQuadHelper(n, lo, mid);
	return RaizQuadHelper(n, mid + 1, hi);
}

int RaizQuad(int n) {
	// assume n >= 0
	// retorna floor(sqrt(n))
	if (n == 0) return 0;
	return RaizQuadHelper(n, 0, n);
}

int main() {
	setbuf(stdout, NULL); setbuf(stderr, NULL);
	int n;
	while (scanf("%d", &n)>0) {
		if (n>=0) {
			/* teste manual */
			printf("%d\n", RaizQuad(n));
		} else {
			/* teste automático */
			n = 50000;
			for (int t=1; t<=200; t++) RaizQuad(n);
			printf("%d\n", RaizQuad(n));
		}
	}

	return 0;
}
