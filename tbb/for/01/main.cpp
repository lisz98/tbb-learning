#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <vector>
#include <cmath>
#include <tbb/task_group.h>


int main(){
    size_t n = 1 <<26;
    std::vector<float> a(n);

    size_t maxt =4;
    tbb::task_group tg;
    for(size_t t=0; t< maxt;++t){
        auto beg = t * n / maxt;
        auto end = std::min(n,(t +1 ) *n /maxt);
        //将区间为(0,n]的数分为4份，分别使用一个线程去进行计算
        tg.run([&,beg,end]{
            for(size_t i =beg ; i < end; i++){
                a[i] = std::sin(i);
            }
        });
    }

    tg.wait();

    return 0;
}
