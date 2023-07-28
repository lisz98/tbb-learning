#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <tbb/parallel_for.h>
#include <tbb/task_arena.h>
#include <tbb/blocked_range2d.h>
#include "ticktock.h"

constexpr size_t n = 1 << 14;

int main(){
    std::vector<float> a(n *n);
    std::vector<float> b(n *n);

    TICK(for);
    size_t grain = 16;
    //按照grain进行切块
    tbb::parallel_for(tbb::blocked_range2d<size_t>(0,n,grain,0,n,grain),[&](tbb::blocked_range2d<size_t> r){
        for(size_t i = r.cols().begin();i < r.cols().end();++i){
            for(size_t j = r.rows().begin(); j < r.rows().end();++j){
                b[i * n + j] = a[j * n +i];
            }
        }
    },tbb::simple_partitioner{});
    TOCK(for);
    return 0;
}
