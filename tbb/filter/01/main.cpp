#include <iostream>
#include <vector>
#include <atomic>
#include <cmath>
#include <mutex>
#include <ticktock.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>
#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>
#include <tbb/tick_count.h>
#include "pod.h"
constexpr size_t n = 1 << 26;

template<class Func>
auto ticktock(Func &&f){
    auto start = tbb::tick_count().now();
    f();
    return (tbb::tick_count().now() -start).seconds();
}
// 普通版本的过滤数据
void normal_filter(){
    std::vector<float> a;
    TICK(normal_filter);
    for(size_t i =0 ; i < n;++i){
        float val = std::sin(i);
        if(val > 0){
            a.push_back(val);
        }
    }
    TOCK(normal_filter);
}

// 加速比只有大约1.7倍，没有理想的4倍
// TODO:
// 可能是因为容器内部只是简单的上锁了，保证了线程的安全，但是效率就不是很高了
void parallel_for_1_filter(){
    tbb::concurrent_vector<float> a;
    TICK(parallel_for_filter);
    tbb::parallel_for(tbb::blocked_range<size_t>(0,n),[&](tbb::blocked_range<size_t> r){
        for(size_t i = r.begin(); i < r.end(); ++i){
            float val = std::sin(i);
            if(val > 0){
                a.push_back(val);
            }
        }
    });
    TOCK(parallel_for_filter);
}

// 采用 一个线程局部的 容器先计算数据，最后再把数据一次性添加到队列中
void parallel_for_2_filter(){
    tbb::concurrent_vector<float> a;
    TICK(parallel_for_filter);
    tbb::parallel_for(tbb::blocked_range<size_t>(0,n),[&](tbb::blocked_range<size_t> r){
        // thread_local 是 c++11 的新特性，每个线程都有一个独立的tmp容器
        // 现将数据添加到tmp容器中，最后在添加到 a容器中
        // 使用 thread_local 特性的话需要使用，static_partitioner 特性，否则会出现别的线程来抢占任务，出现死锁
        thread_local std::vector<float> tmp;
        for(size_t i = r.begin(); i < r.end(); ++i){
            float val = std::sin(i);
            if(val > 0){
                tmp.push_back(val);
            }
        }

        auto it = a.grow_by(tmp.size());
        //for(size_t i =0; i < tmp.size();++i ){
            //*it++= tmp[i];
        //}
        std::copy(tmp.begin(),tmp.end(),it);
    },tbb::static_partitioner{});
    TOCK(parallel_for_filter);
}

void parallel_for_3_filter(){
    tbb::concurrent_vector<float> a;
    TICK(parallel_for_filter);
    tbb::parallel_for(tbb::blocked_range<size_t>(0,n),[&](tbb::blocked_range<size_t> r){
        // thread_local 是 c++11 的新特性，每个线程都有一个独立的tmp容器
        // 现将数据添加到tmp容器中，最后在添加到 a容器中
        std::vector<float> tmp;
        tmp.reserve(r.size());
        for(size_t i = r.begin(); i < r.end(); ++i){
            float val = std::sin(i);
            if(val > 0){
                tmp.push_back(val);
            }
        }

        auto it = a.grow_by(tmp.size());
        std::copy(tmp.begin(),tmp.end(),it);
    },tbb::auto_partitioner{});
    TOCK(parallel_for_filter);
} 

void parallel_for_4_filter(){
    // 需要筛选的容器是连续的,则需要配合锁来进行编程
    std::vector<float> a;
    a.reserve(n);
    std::recursive_mutex mtx;
    TICK(parallel_for_filter);
    tbb::parallel_for(tbb::blocked_range<size_t>(0,n),[&](tbb::blocked_range<size_t> r){
        // thread_local 是 c++11 的新特性，每个线程都有一个独立的tmp容器
        // 现将数据添加到tmp容器中，最后在添加到 a容器中
        std::vector<float> tmp;
        tmp.reserve(r.size());
        for(size_t i = r.begin(); i < r.end(); ++i){
            float val = std::sin(i);
            if(val > 0){
                tmp.push_back(val);
            }
        }

        std::lock_guard lock(mtx);
        std::copy(tmp.begin(),tmp.end(),std::back_inserter(a));
    },tbb::auto_partitioner{});
    TOCK(parallel_for_filter);
} 
void parallel_for_5_filter(){
    // 需要筛选的容器是连续的,则需要配合锁来进行编程
    std::vector<float> a;
    a.reserve(n * 2 /3);
    std::recursive_mutex mtx;
    TICK(parallel_for_filter);
    tbb::parallel_for(tbb::blocked_range<size_t>(0,n),[&](tbb::blocked_range<size_t> r){
        // thread_local 是 c++11 的新特性，每个线程都有一个独立的tmp容器
        // 现将数据添加到tmp容器中，最后在添加到 a容器中
        std::vector<float> tmp;
        tmp.reserve(r.size());
        for(size_t i = r.begin(); i < r.end(); ++i){
            float val = std::sin(i);
            if(val > 0){
                tmp.push_back(val);
            }
        }

        std::lock_guard lock(mtx);
        std::copy(tmp.begin(),tmp.end(),std::back_inserter(a));
    },tbb::auto_partitioner{});
    TOCK(parallel_for_filter);
}

void parallel_for_6_filter(){
    // 需要筛选的容器是连续的,则需要配合锁来进行编程
    std::vector<pod<float>> a;
    // 使用pod类型使std::vector 预分配数据的时候不直接进行初始化数据为0
    std::atomic<size_t> a_size = 0;
    TICK(parallel_for_filter);
    a.resize(n);
    tbb::parallel_for(tbb::blocked_range<size_t>(0,n),[&](tbb::blocked_range<size_t> r){
        // thread_local 是 c++11 的新特性，每个线程都有一个独立的tmp容器
        // 现将数据添加到tmp容器中，最后在添加到 a容器中
        std::vector<pod<float>> tmp(r.size());
        size_t lasize =0;
        for(size_t i = r.begin(); i < r.end(); ++i){
            float val = std::sin(i);
            if(val > 0){
                tmp[lasize++] += val;
            }
        }
        size_t base = a_size.fetch_add(lasize);
        for(size_t i =0; i <lasize;++i){
            a[base +i] = tmp[i];
        }
    },tbb::auto_partitioner{});
    a.resize(a_size);
    TOCK(parallel_for_filter);
}

void parallel_for_7_filter(){
    TICK(parallel_for_filter);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0,n),std::vector<float>{},
                      [&](tbb::blocked_range<size_t> r,std::vector<float> local){
                          local.reserve(local.size() + r.size());
                          for(size_t i = r.begin(); i < r.end(); ++i){
                              float val = std::sin(i);
                              if(val > 0){
                                  local.push_back(val);
                              }
                          }
                          return local;
    },[](std::vector<float> a,std::vector<float> const &b){
        std::copy(b.begin(),b.end(),std::back_inserter(a));
        return a;
    });
    TOCK(parallel_for_filter);
}

int main(){
    auto normal = ticktock(normal_filter);
    double parallel_for; 
    parallel_for  = ticktock(parallel_for_1_filter);
    std::cout << "parallel for 1 speed-up ratio: " <<  normal / parallel_for << "ratio" << std::endl;
    parallel_for   = ticktock(parallel_for_2_filter);
    std::cout << "parallel for 2 speed-up ratio: " <<  normal / parallel_for << "ratio" << std::endl;
    parallel_for   = ticktock(parallel_for_3_filter);
    std::cout << "parallel for 3 speed-up ratio: " <<  normal / parallel_for << "ratio" << std::endl;
    parallel_for   = ticktock(parallel_for_4_filter);
    std::cout << "parallel for 4 speed-up ratio: " <<  normal / parallel_for << "ratio" << std::endl;
    parallel_for   = ticktock(parallel_for_5_filter);
    std::cout << "parallel for 5 speed-up ratio: " <<  normal / parallel_for << "ratio" << std::endl;
    parallel_for   = ticktock(parallel_for_6_filter);
    std::cout << "parallel for 6 speed-up ratio: " <<  normal / parallel_for << "ratio" << std::endl;
    parallel_for   = ticktock(parallel_for_7_filter);
    std::cout << "parallel for 7 speed-up ratio: " <<  normal / parallel_for << "ratio" << std::endl;

    return 0;
}

