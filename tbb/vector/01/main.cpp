#include <iostream>
#include <vector>
#include <cmath>
#include <tbb/concurrent_vector.h>

int main(){
    size_t n = 1 <<10;
    std::vector<float*> pa(n);
    // 观察现象发现，前半段的数据，因为vector发生扩容，而导致pa存储的指针并不是a中相应的指针了
    // 这时候就会导致，迭代器失效，导致数据错误
    // 1. 可以使用预留的方式解决这个问题
    //a.reserve(n);
    // 2. 也可以使用tbb::concurrent_vector 来解决这个问题
    //tbb::concurrent_vector 不保证元素在内存中的地址是连续的，所以使用push_back插入后，不需要扩容而移动位置，也就不会出现指针和迭代器失效了
#if 0
    std::vector<float> a;
    for(size_t i = 0; i < n;++i){
        a.push_back(std::sin(i));
        pa[i] = &a.back();
    }
#else
    tbb::concurrent_vector<float> a;
    for(size_t i = 0; i < n;++i){
        // 调用push_back会返回一个迭代器
        tbb::concurrent_vector<float>::iterator it = a.push_back(std::sin(i));
        pa[i] = &*it;
    }

#endif

    for(size_t i =0; i < n; ++i){
        std::cout <<(&a[i] == pa[i])<<' ';
    }
    std::cout << std::endl;
    return 0;
}
