#include<iostream>
#include <vector>
#include <algorithm>

int mergeSort(std::vector<long long int>& sum, int lower, int upper, int low, int high)
{
    if(high-low <= 1) return 0;
    int mid = (low+high)/2, m = mid, n = mid, count =0;
    count =mergeSort(sum,lower,upper,low,mid) +mergeSort(sum,lower,upper,mid,high);
    for(int i =low; i< mid; i++)
    {
        while(m < high && sum[m] - sum[i] < lower) m++;
        while(n < high && sum[n] - sum[i] <= upper) n++;
        count += n - m;
    }
    std::inplace_merge(sum.begin()+low, sum.begin()+mid, sum.begin()+high);
    return count;
}

int countRangeSum(std::vector<int>& nums, int lower, int upper) {
    int len = nums.size();
    std::vector<long long int> sum(len + 1, 0);
    for(int i =0; i< len; i++) sum[i+1] = sum[i]+nums[i];
    return mergeSort(sum, lower, upper, 0, len+1);
}

int main(){
	//get inputs
    int n, lower, upper;
    std::vector<int> nums;

    std::cin >> n>> lower >> upper;

    int tmp;
    for (int i=0; i<n; i++) {
        std::cin >> tmp;
        nums.push_back(tmp);
    }

    int count = countRangeSum(nums, lower, upper);

    
    std::cout<< count <<std::endl;

    return 0;
}