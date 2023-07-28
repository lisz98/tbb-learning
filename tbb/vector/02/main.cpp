#include <iostream>
#include <vector>
#include <cmath>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>

int main(){
    constexpr size_t n = 5;
    tbb::concurrent_vector<float> a;

    for(size_t i =0; i< n; ++i){
        auto it = a.grow_by(4);
        for(int j =0; j < 4;++j){
            *it++ = std::cos(i + std::rand());
        }
    }

    // 三种访问方式
    // 不建议使用随机访问的方式访问，建议使用迭代器进行访问
    for(size_t i =0; i < a.size(); ++i){
        a[i] += 1.0f;
    }

    for(tbb::concurrent_vector<float>::value_type it : a){
        it -= 1.0f;
    }

    for(auto it = a.begin(); it != a.end(); ++it){
        *it -= 1.0f;
    }   


    tbb::parallel_for(tbb::blocked_range(a.begin(),a.end()),[&](auto r){
        for(auto it = r.begin(); it < r.end(); ++it){
            std::cout << *it << ' ' ;
        }
    });
    std::cout << std::endl;

    return 0;
}
