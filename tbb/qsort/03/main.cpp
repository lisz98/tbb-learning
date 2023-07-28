#include <iostream>
#include <cmath>
#include <ticktock.h>
#include <tbb/task_group.h>
#include <tbb/parallel_invoke.h>
#include <tbb/parallel_reduce.h>
#include <numeric>
#include <algorithm>
#include <tbb/blocked_range.h>
#include <iterator>

// 并行缩并
// 将大问题一分为二，再到足够小的时候，再转为串行
template<class T>
T quick_reduce(T *data, size_t size){
    if(size < (1 <<16)){
        return std::reduce(data,data + size);
    }
    T sum1 , sum2;
    size_t mid = size /2;
    tbb::parallel_invoke([&](){
        sum1 = quick_reduce(data,mid);
    },[&](){
        sum2 = quick_reduce(data +mid , size -mid);
    });
    return sum1 + sum2;
}

template<class T>
T tbb_reduce(T *data, size_t size){
    T sum =tbb::parallel_reduce(tbb::blocked_range<size_t>(0,size), T(), [&](tbb::blocked_range<size_t> r,T res){
        for(size_t i = r.begin(); i < r.end();++i){
            res += data[i];
        }
        return res;
    },std::plus<T>{});
    return sum;
}    

int main()
{
    size_t n = 1 <<25;
    std::vector<int> arr(n);
    std::generate(arr.begin(),arr.end(),std::rand);
    TICK(quick_reduce);
    int sum = quick_reduce(arr.data(),arr.size());
    TOCK(quick_reduce);
    std::cout << sum << std::endl;


    TICK(tbb_reduce);
    sum = tbb_reduce(arr.data(),arr.size());
    TOCK(tbb_reduce);
    std::cout << sum << std::endl;
    return 0;
}
