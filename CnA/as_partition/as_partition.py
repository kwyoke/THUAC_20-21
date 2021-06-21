'''
This script computes the number of partitions of integer n with such that the length of each partition <=k.

Idea:
By taking conjugates, the number pk(n) of partitions of n into exactly k parts 
is equal to the number of partitions of n in which the largest part has size k. 

The function pk(n) satisfies the recurrence:
pk(n) = pk(n − k) + pk−1(n − 1)
with initial values p0(0) = 1 and pk(n) = 0 if n ≤ 0 or k ≤ 0 and n and k are not both zero.
'''

import sys

n_max = 2001
#init memo, memo[n][k]
memo = [[0 for x in range(n_max)] for y in range(n_max)]
for i in range(n_max):
	memo[i][0] = 0 
for j in range(n_max):
	memo[0][j] = 1


#k is the largest element allowed in each partition
#k is also the length of the longest set of partitions allowed
def num_partition(n, k):
	for i in range(1, n+1):
		for j in range(1, k+1):
			if i<j:
				memo[i][j] = memo[i][j-1]
			else:
				memo[i][j] = memo[i][j-1] + memo[i-j][j]
	return memo[n][k]


if __name__ == '__main__':
    n,k = sys.stdin.readline().split(' ')
    n = int(n)
    k = int(k)
    
    ans = num_partition(n,k)
    ans = ans%(10**9 + 7)

    print(ans)