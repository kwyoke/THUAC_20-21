/*
	This program calculates C(a,b),
	and outputs the answer modulo (10^9 + 7)
*/

#include<cstdlib>
#include<cstdio>
#include<iostream>
#include <cmath>

long long int find_gcd(long long int m, long long int n) {
	while (m != n) {
		if (m > n) {
			m -= n;
		}
		else {
			n -= m;
		}
	return n;
	}
}

int main(){
    int a,b;

    //1 ≤ b ≤ a ≤ 2000
    std::cin>>a>>b;

    //strategy: C(a, b) = [a * (a-1) * .... * (a-[b-1])] / [b * (b-1) * .... * 1]
    //b terms on numerator, b terms on denominator
    
    //initialise output holders
    long long int numerator = 1;
    long long int denominator = 1;
    long long int gcd;
    long long int modulo = std::pow(10, 9) + 7;

    if (b > a-b) {
    	b = a - b;
    }

    for (int i=0; i<b; i++) {
    	numerator *= (a-i);
    	denominator *= (i+1);

    	//use Greatest Common Divisor (gcd) to prevent numbers from overflowing
    	gcd = find_gcd(numerator, denominator);
    	numerator /= gcd;
    	numerator %= modulo;
    	denominator /= gcd;
    	denominator %= modulo;
    }

    std::cout<<numerator%modulo <<std::endl;
    return 0;
}
