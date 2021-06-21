/*
	This program calculates C(a,b),
	and outputs the answer modulo (10^9 + 7)
*/

#include<iostream>
#include <cmath>

//uses dynamic programming, making use of 
// C(n,r) = C(n-1, r-1) + C(n-1, r)
// also makes use of distributive property of modulo for addition
int nCr_mod(int n, int r) {
	int modulo = std::pow(10, 9) + 7;

	if (r > n-r) {
		r = n-r;
	}

	//initialisation of memo
	int C[n+1][r+1] = {0}; 
	for (int i=0; i<=n; i++) {
		C[i][0] = 1;

		if (i<=r) {
			C[i][i] = 1;
		}
	}

	for (int i=1; i<=n; i++) {
		for (int j=1; j<=r; j++) {
			if (i>j) {
				C[i][j] = (C[i-1][j-1] + C[i-1][j])%modulo;
			}
		}
	}

	return C[n][r];

}


int main(){
    int a,b;

    //1 ≤ b ≤ a ≤ 2000
    std::cin>>a>>b;

    std::cout<< nCr_mod(a, b) <<std::endl;
    return 0;
}
