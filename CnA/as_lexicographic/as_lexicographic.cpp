/*
	This program calculates (k+a)th permutation in lexicographic order,
	given the kth permutation
*/

#include<iostream>
#include <vector>
#include <algorithm>

unsigned long long int factorial_table[20] = {0}; //table to store factorial results up to n=19

//find the lexicographic order k of the input sequence
unsigned long long int find_order_of_perm(int seq[], int n) {
	int multipliers[n-1] = {0}; //multipliers to each (n-i)!
	multipliers[0] = seq[0] - 1;

	//find subsequent multipliers
	for (int i=1; i<n-1; i++) {
		multipliers[i] = seq[i] - 1; //(seq[i]-1) possible digits smaller than seq[i]

		//decrement multiplier if digits on the left decrease possibilities
		for (int j=0; j<i; j++) {
			if (seq[j] < seq[i]) {
				multipliers[i]--;
			}
		}
	}

	unsigned long long int k=0; //kth order
	for (int i=0; i<n-1; i++) {
		k += multipliers[i]*factorial_table[n-1-i];
	}
	k+=1;
	return k;
}


//compute the kth permutation in lexicographic order of n-digits
void kth_perm(unsigned long long int k, int n, int seq[]) {
	std::vector<int> available_digits; //vector to track digits available, in order
	for (int i=0; i<n; i++) {
		available_digits.push_back(i+1);
	}

	unsigned long long int remainder = k-1; //k-1 permutations before the kth one
	int quotient;
	std::vector<int>::iterator new_end;

	for (int i=0; i<n; i++) {
		quotient = remainder/factorial_table[n-1-i];
		
		seq[i] = available_digits[quotient];
		//std::cout<< remainder << " " << factorial_table[n-1-i] << " " << quotient << " " << seq[i] << std::endl;
		new_end = std::remove(available_digits.begin(), available_digits.end(), seq[i]);
		remainder = remainder % factorial_table[n-1-i];
	}
	
}


int main(){
	
	//init factorial table
	factorial_table[0] = 1; //base case
	for (int i=1; i<20; i++) {
		factorial_table[i] = factorial_table[i-1] * i;
	}

	//get inputs
    int n;
    unsigned long long int a;

    std::cin>>n>>a;

    int sequence[n] = {0};

    for (int i=0; i<n; i++) {
    	std::cin >> sequence[i];
    }

    //Find the lexicographic order number of the input sequence
    unsigned long long int k = find_order_of_perm(sequence, n);

    //Desired order (k+a)th
    unsigned long long int desired_order = k + a;

    //Find (k+a)th permutation in lexicographic order
    int out_seq[n] = {0}; //out_seq passed by ref to function
    kth_perm(desired_order, n, out_seq);

    //print output
    for (int i=0; i<n-1; i++) {
    	std::cout << out_seq[i] << " ";
    }
    std::cout<< out_seq[n-1] <<std::endl;

    return 0;
}