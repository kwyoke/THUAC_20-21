/*
	This program calculates the kth n-digit-permutation in the SJT sequence
*/
#include<iostream>


void find_kth_perm(int n, unsigned long long int k, int kth_perm[]) {
	//set up base case
	if (n==1) {
		kth_perm[0] = 1;
	}
	else if (n==2) {
		if (k==1) {
			kth_perm[0] = 1;
			kth_perm[1] = 2;
		}
		else if (k==2) {
			kth_perm[0] = 2;
			kth_perm[1] = 1;
		}
	}
	//recursion
	else {
		unsigned long long int q = k/n; //quotient
		int r = k%n; //remainder

		unsigned long long int b; //block id which corresponds to bth perm of (n-1) SJT sequence
		int i; //index within each block, that tells us position to put n in
		if (r==0) {
			b = q-1; 
			i = n-1; 
		}
		else {
			b = q;
			i = r-1;
		}

		int pos; //position to put digit n in
		if (b%2==0) {
			pos = n-1-i; //fill in n from right
		}
		else {
			pos = i; //fill in n from left
		}

		int corr_old_perm[n-1] = {0};
		find_kth_perm(n-1, b+1, corr_old_perm);

		kth_perm[pos] = n;
		int pos_old_perm = 0;
		for (int p=0; p<n; p++) {
			if (p != pos) {
				kth_perm[p] = corr_old_perm[pos_old_perm];
				pos_old_perm++;
			}
		}
	}

}


int main(){
	//get inputs
    int n;
    unsigned long long int k;

    std::cin>>n>>k;

    int out_seq[n] = {0};
    find_kth_perm(n, k, out_seq);

    //print output
    for (int i=0; i<n-1; i++) {
    	std::cout << out_seq[i] << " ";
    }
    std::cout<< out_seq[n-1] <<std::endl;

    return 0;
}