import sys

def countRangeSum(nums, lower, upper):
    n = len(nums)
    sums = []
    sums.append(0)
    for i in range(n):
        sums.append(sums[i] + nums[i])
    return countMergeSort(sums, 0, n+1, lower, upper)


def countMergeSort(sums, start, end, lower, upper):
    if (end-start <= 1):
        return 0
    
    mid = start + (end-start)//2
    j = k = t = mid
    
    count = countMergeSort(sums, start, mid, lower, upper) + countMergeSort(sums, mid, end, lower, upper)

    small_sum_store = []
    for i in range(start, mid):
        while (k < end) and (sums[k] - sums[i] < lower):
            k+=1
        while (j < end) and (sums[j] - sums[i] <= upper):
            j+=1
        while (t < end) and (sums[t] < sums[i]):
            small_sum_store.append(sums[t])
            t+=1
        small_sum_store.append(sums[i])
        count += (j-k)

    for i in range(t-start):
        sums[i + start] = small_sum_store[i]
    
    return count

if __name__ == '__main__':
	n, lower, upper = sys.stdin.readline().split(' ')
	n = int(n)
	lower = int(lower)
	upper = int(upper)
	
	nums = [int(x) for x in sys.stdin.readline().split(' ')]
	
	print(countRangeSum(nums, lower, upper))