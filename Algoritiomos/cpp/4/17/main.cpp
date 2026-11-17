#include <cstdio>
#include <cstdlib>

long long int NumeroCaminhos(int n, int m, int X[], int Y[]) {
	// Grid is nxn, robot starts at (1,1) goes to (n,n)
	// Can move right or down, but (X[i],Y[i]) pairs are forbidden
	// Use recursion with memoization on a grid
	// Forbidden cells: blocked[X[i]][Y[i]] = true
	bool * blocked = (bool *) calloc((n+1)*(n+1), sizeof(bool));
	for (int i = 0; i < m; i++) {
		if (X[i] >= 1 && X[i] <= n && Y[i] >= 1 && Y[i] <= n) {
			blocked[X[i]*(n+1)+Y[i]] = true;
		}
	}
	long long int * memo = (long long int *) malloc(sizeof(long long int) * (n+1)*(n+1));
	for (int i = 0; i < (n+1)*(n+1); i++) memo[i] = -1;

	long long int CaminhosRec(int x, int y, int n, bool * blocked, long long int * memo);

	long long int result = CaminhosRec(1, 1, n, blocked, memo);
	free(blocked); free(memo);
	return result;
}

long long int CaminhosRec(int x, int y, int n, bool * blocked, long long int * memo) {
	if (x > n || y > n) return 0;
	if (blocked[x*(n+1)+y]) return 0;
	if (x == n && y == n) return 1;
	if (memo[x*(n+1)+y] != -1) return memo[x*(n+1)+y];
	long long int result = CaminhosRec(x+1, y, n, blocked, memo) + CaminhosRec(x, y+1, n, blocked, memo);
	memo[x*(n+1)+y] = result;
	return result;
}

int main() {
	setbuf(stdout, NULL); setbuf(stderr, NULL);
	int n; int m; int * X; int * Y;
	while (scanf("%d %d", &n, &m)>0) {
		if (n>=0) {
			/* teste manual */
			X = (int *) malloc(sizeof(int)*m); Y = (int *) malloc(sizeof(int)*m);
			for (int i=0; i<m; i++) scanf("%d", &X[i]);
			for (int i=0; i<m; i++) scanf("%d", &Y[i]);
			printf("%lld\n", NumeroCaminhos(n,m,X,Y));
			free(X); free(Y);
		} else {
			/* teste automático */
			n = 2000; m=2*n;
			X = (int *) malloc(sizeof(int)*m); Y = (int *) malloc(sizeof(int)*m);
			m = 0;
			for (int i=2; i<=n; i++) {
				X[m] = i; Y[m] = i-1; m++;
				X[m] = i-1; Y[m] = i+1; m++;
			}
			printf("%lld\n", NumeroCaminhos(n,m,X,Y));
			free(X); free(Y);
		}
	}

	return 0;
}
