import sys

def countRangeSum(nums, lower, upper):
    n = len(nums)
    sums = []
    sums.append(0)
    for i in range(n):
        sums.append(sums[i] + nums[i])
    return countMergeSort(sums, lower, upper)

def countMergeSort(sums, lower, upper):
    if (len(sums) <= 1):
        return 0
    
    mid = len(sums)//2
    L = sums[:mid]
    R = sums[mid:]
    print(sums)
    count = countMergeSort(L, lower, upper) + countMergeSort(R, lower, upper) #find rangesums in left and right, plus s
    print(sums)

    j = k = t = mid

    small_sums_store = []
    for i in range(mid):
        while (k < len(sums) and (sums[k] - sums[i] < lower)):
            k+=1
        while (j < len(sums) and (sums[j] - sums[i] <= upper)):
            j+=1
        while (t < len(sums) and (sums[t] < sums[i])):
            small_sums_store.append(sums[t]) #for sorting sums later
            t += 1
        small_sums_store.append(sums[i])
        count += (j-k)

    #print(sums)
    for i in range(t): #sort sums in increasing order
        sums[i] = small_sums_store[i] 

    #print(sums)
    return count


if __name__ == '__main__':
    n, lower, upper = sys.stdin.readline().split(' ')
    n = int(n)
    lower = int(lower)
    upper = int(upper)
    
    nums = [int(x) for x in sys.stdin.readline().split(' ')]
    
    print(countRangeSum(nums, lower, upper))