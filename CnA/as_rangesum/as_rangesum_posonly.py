import sys

# Python 3 program to find 
# number of subarrays having 
# sum in the range L to R. 

# Function to find number 
# of subarrays having sum 
# less than or equal to x. 
def countSub(arr, n, x): 
	
	# Starting index of 
	# sliding window. 
	st = 0

	# Ending index of 
	# sliding window. 
	end = 0

	# Sum of elements currently 
	# present in sliding window. 
	sum = 0

	# To store required 
	# number of subarrays. 
	cnt = 0

	# Increment ending index 
	# of sliding window one 
	# step at a time. 
	while end < n : 
		
		# Update sum of sliding 
		# window on adding a 
		# new element. 
		sum += arr[end] 

		# Increment starting index 
		# of sliding window until 
		# sum is greater than x. 
		while (st <= end and sum > x) : 
			sum -= arr[st] 
			st += 1

		# Update count of 
		# number of subarrays. 
		cnt += (end - st + 1) 
		end += 1

	return cnt 

# Function to find number 
# of subarrays having sum 
# in the range L to R. 
def findSubSumLtoR(arr, n, L, R): 
	
	# Number of subarrays 
	# having sum less 
	# than or equal to R. 
	Rcnt = countSub(arr, n, R) 

	# Number of subarrays 
	# having sum less than 
	# or equal to L-1. 
	Lcnt = countSub(arr, n, L - 1) 

	return Rcnt - Lcnt 

# This code is contributed 
# by ChitraNayal 

if __name__ == '__main__':
	n, lower, upper = sys.stdin.readline().split(' ')
	n = int(n)
	lower = int(lower)
	upper = int(upper)
	
	nums = [int(x) for x in sys.stdin.readline().split(' ')]
	
	print(findSubSumLtoR(nums, n, lower, upper))