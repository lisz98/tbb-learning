#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <tbb/parallel_for.h>
#include <tbb/task_arena.h>
#include <tbb/blocked_range.h>
#include "ticktock.h"

constexpr size_t n = 64;

void tbb_print(size_t i){
#if 0
    std::cout << "tbb thread index : " <<tbb::this_task_arena::current_thread_index() << std::endl
        << "blocked_range size =" << i << std::endl;
#else
    (void)(i);
#endif
}

// 使用static_partitioner对任务进行分块
// 创建四个线程四个任务，每个任务切分size的64 /4 =16个数据
// 使用32 指定每个任务包含32个数据，这时候只会分配两个线程,tbb::blocked_range<size_t>(0,n,32)
void static_partitioner_test(){
    TICK(static_partitioner_test);
    std::vector<float> a(n);
    tbb::task_arena ta(tbb::this_task_arena::max_concurrency());
    ta.execute([&](){
        tbb::parallel_for(tbb::blocked_range<size_t>(0,n,32), [&](tbb::blocked_range<size_t> r){
            tbb_print(r.size());
            for(size_t i =  r.begin(); i < r.end(); ++i){
                a[i] = std::sin(i) + std::cos(i);
            }
        },tbb::static_partitioner{});
    });
    TOCK(static_partitioner_test);
}

// 使用 auto_partitioner对任务进行自动分块
// 一般情况下这都是一种最优解，tbb会自动帮你分配策略
void auto_partitioner_test(){
    TICK(auto_partitioner_test);
    std::vector<float> a(n);
    tbb::task_arena ta(tbb::this_task_arena::max_concurrency());
    ta.execute([&](){
        tbb::parallel_for(tbb::blocked_range<size_t>(0,n), [&](tbb::blocked_range<size_t> r){
            tbb_print(r.size());
            for(size_t i =  r.begin(); i < r.end(); ++i){
                a[i] = std::sin(i) + std::cos(i);
            }
        },tbb::auto_partitioner{});
    });
    TOCK(auto_partitioner_test);
}

// 使用 simple_partitioner 对任务进行分块
// 指定区间的颗粒度
// 创建四个线程64 /8 =8个任务，每个任务4个元素
void simple_partitioner_test(){
    TICK(simple_partitioner_test);
    std::vector<float> a(n);
    tbb::task_arena ta(tbb::this_task_arena::max_concurrency());
    ta.execute([&](){
        tbb::parallel_for(tbb::blocked_range<size_t>(0,n,4), [&](tbb::blocked_range<size_t> r){
            tbb_print(r.size());
            for(size_t i =  r.begin(); i < r.end(); ++i){
                a[i] = std::sin(i) + std::cos(i);
            }
        },tbb::simple_partitioner{});
    });
    TOCK(simple_partitioner_test);
}
// 使用 affinity_partitioner 对任务进行分块
// 记录历史，下次根据经验自动负载均衡
void affinity_partitioner_test(){
    TICK(affinity_partitioner_test);
    std::vector<float> a(n);
    tbb::task_arena ta(tbb::this_task_arena::max_concurrency());
    ta.execute([&](){
        tbb::affinity_partitioner affinity;
        tbb::parallel_for(tbb::blocked_range<size_t>(0,n), [&](tbb::blocked_range<size_t> r){
            tbb_print(r.size());
            for(size_t i =  r.begin(); i < r.end(); ++i){
                a[i] = std::sin(i) + std::cos(i);
            }
        },affinity);
    });
    TOCK(affinity_partitioner_test);
}
int main(){
    static_partitioner_test();
    auto_partitioner_test();
    simple_partitioner_test();
    affinity_partitioner_test();
    return 0;
}
