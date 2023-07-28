#include <iostream>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <tbb/parallel_invoke.h>
#include <tbb/parallel_sort.h>
#include "ticktock.h"

constexpr size_t n = 1 << 25;

template<class T>
void quick_sort(T *data , size_t size){
    if(size <1){
        return ;
    }

    // 使用hash函数来创建随机的mid
    // 使用hash防止数据已经有序的情况下，造成最坏的O(n2)的情况

    // 来自chatGpt
    //这段代码是用于计算一个哈希值的过程。让我逐步解释每一行代码的含义：
    //1. `size_t mid = std::hash<size_t>{}(size);`
    //这一行代码将变量 `size` 的哈希值计算出来，并将结果赋值给变量 `mid`。`std::hash<size_t>{}` 是一个哈希函数对象，通过调用它并传入 `size` 参数来计算哈希值。

    //2. `std::hash<void *>{}(static_cast<void *>(data))`
    //计算出了变量`data`的哈希值。而`mid ^= std::hash<void *>{}(static_cast<void *>(data))`是将变量`mid`与该哈希值进行按位异或操作（XOR）。按位异或操作是一种位运算，它对两个操作数的每个对应位执行异或操作，如果两个对应位的值相同，则结果为0，否则结果为1。在这段代码中，通过将变量`mid`与`data`的哈希值进行异或操作，可以将哈希值的信息混合到`mid`中，从而增加`mid`的随机性和唯一性。这样做的目的可能是为了在计算哈希值的基础上进一步增加数据的随机性，以提高哈希算法的效果。

    //3. `mid %= size;`
    //这一行代码将变量 `mid` 除以 `size` 并取余数，然后将结果赋值给变量 `mid`。这一步是为了确保哈希值在指定的范围内，以便在需要时进行索引或映射。

    //总体而言，这段代码的目的是计算一个哈希值，它结合了 `size` 和 `data` 两个变量的哈希值，并对结果进行一系列的操作，最终得到一个在指定范围内的哈希值。这种哈希值的计算常用于数据结构中的索引或映射操作，以提高数据的查找效率。    
    size_t mid = std::hash<size_t>{}(size);
    mid ^= std::hash<void *>{}(static_cast<void *>(data));
    mid %= size;
    std::swap(data[0],data[mid]);
    T pivot = data[0];
    size_t left =0, right = size -1;
    while(left < right){
        while(left < right && !(data[right] < pivot)){
            right--;
        }
        if(left < right){
            data[left++] = data[right];
        }
        while(left < right && !(data[left] < pivot)){
            left++;
        }
        if(left < right){
            data[right--] = data[left];
        }
    }
    data[left] = pivot;
    quick_sort(data,left);
    quick_sort(data + left+1,size -left -1);
}

template<class T>
void quick_sort_parallel(T *data , size_t size){
    if(size <1){
        return ;
    }

    // 使用hash函数来创建随机的mid
    // 使用hash防止数据已经有序的情况下，造成最坏的O(n2)的情况
    size_t mid = std::hash<size_t>{}(size);
    mid ^= std::hash<void *>{}(static_cast<void *>(data));
    mid %= size;
    std::swap(data[0],data[mid]);
    T pivot = data[0];
    size_t left =0, right = size -1;
    while(left < right){
        while(left < right && !(data[right] < pivot)){
            right--;
        }
        if(left < right){
            data[left++] = data[right];
        }
        while(left < right && !(data[left] < pivot)){
            left++;
        }
        if(left < right){
            data[right--] = data[left];
        }
    }
    data[left] = pivot;
    tbb::parallel_invoke([&](){
        quick_sort_parallel(data,left);
    },[&](){
        quick_sort_parallel(data + left+1,size -left -1);
    });
}

template<class T>
void quick_sort_parallel_and_std(T *data , size_t size){
    if(size <1){
        return ;
    }
    if(size < (1 <<16)){
        std::sort(data,data+size,std::less<T>{});
        return;
    }

    // 使用hash函数来创建随机的mid
    // 使用hash防止数据已经有序的情况下，造成最坏的O(n2)的情况
    size_t mid = std::hash<size_t>{}(size);
    mid ^= std::hash<void *>{}(static_cast<void *>(data));
    mid %= size;
    std::swap(data[0],data[mid]);
    T pivot = data[0];
    size_t left =0, right = size -1;
    while(left < right){
        while(left < right && !(data[right] < pivot)){
            right--;
        }
        if(left < right){
            data[left++] = data[right];
        }
        while(left < right && !(data[left] < pivot)){
            left++;
        }
        if(left < right){
            data[right--] = data[left];
        }
    }
    data[left] = pivot;
    tbb::parallel_invoke([&](){
        quick_sort_parallel(data,left);
    },[&](){
        quick_sort_parallel(data + left+1,size -left -1);
    });
}

auto getVector(){
    static bool is_first = true;
    static std::vector<int> arr(n);
    if(is_first){
        std::generate(arr.begin(),arr.end(),std::rand);
    }
    return std::vector<int>(arr.begin(),arr.end());
}
void quick_sort()
{
    auto arr = getVector();
    TICK(quick_sort);
    quick_sort(arr.data(),arr.size());
    TOCK(quick_sort);
}

void quick_sort_parallel()
{
    auto arr = getVector();
    TICK(quick_sort_parallel);
    quick_sort_parallel(arr.data(),arr.size());
    TOCK(quick_sort_parallel);
}

void quick_sort_parallel_and_std()
{
    auto arr = getVector();
    TICK(quick_sort_parallel_and_std);
    quick_sort_parallel_and_std(arr.data(),arr.size());
    TOCK(quick_sort_parallel_and_std);
}   


void quick_sort_tbb()
{
    auto arr = getVector();
    TICK(quick_sort_tbb);
    tbb::parallel_sort(arr.begin(),arr.end(),std::less<int>());
    TOCK(quick_sort_tbb);
}   

void std_sort()
{
    auto arr = getVector();
    TICK(std_sort);
    std::sort(arr.begin(),arr.end(),std::less<int>());
    TOCK(std_sort);
}

int main(){
    std_sort();
    quick_sort();
    quick_sort_parallel();
    quick_sort_parallel_and_std();
    quick_sort_tbb();
    return 0;
}
