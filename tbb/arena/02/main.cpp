#include <tbb/task_arena.h>
#include <iostream>
#include <tbb/parallel_for.h>
#include <vector>
#include <cmath>
#include <string>
#include <mutex>
#include "ticktock.h"

#define EXECUTE_TYPE 2
#define USE_RECURSIVE_MUTEX
auto get_max_concurrency()
{
    //获取当前的最大核心数
    //获取当前的硬件信息
    auto max_concurrency =tbb::this_task_arena::max_concurrency();
    FILE * file = popen("uname -a","r");
    char buffer[128];
    //std::string msg;
    while(!feof(file)){
        if(fgets(buffer,128,file) != nullptr){
            //msg += buffer;
            fprintf(stdout,"%s",(const char*)buffer);
        }
    }
    fprintf(stdout,"\n");
    fclose(file);
    
    std::cout << "computer max concurrency is :" << max_concurrency << std::endl;
    
    return max_concurrency;
}

// 使用递归锁，防止出现死锁情况
#ifdef USE_RECURSIVE_MUTEX
std::recursive_mutex mtx;
#else
std::mutex mtx;
#endif
int main(){
    size_t n = 1 << 13;
    std::vector<float> a(n *n);
    TICK(area);

#if EXECUTE_TYPE == 1
    // 使用blocked_range的方式，使得编译器有机会进行SMID优化
    tbb::parallel_for(tbb::blocked_range<size_t>(0,n),[&](tbb::blocked_range<size_t> r){
        for(size_t i = r.begin(); i < r.end(); ++i){
            std::lock_guard lock(mtx);
            // 创建独立的任务域,这样就不会出现小偷
            tbb::task_arena task;
            task.execute([&](){
                tbb::parallel_for(tbb::blocked_range<size_t>(0,n),[&](tbb::blocked_range<size_t> h){
                    for(size_t j = h.begin(); j < h.end(); ++j){
                        a[i * n + j] = std::sin(i) * std::sin(j);
                    }
                });
            });
        }
    });
#elif EXECUTE_TYPE ==2
    tbb::parallel_for(tbb::blocked_range<size_t>(0,n),[&](tbb::blocked_range<size_t> r){
        for(size_t i = r.begin(); i < r.end(); ++i){
            std::lock_guard lock(mtx);
            //创建在同一个任务域中，并禁止其任务被别的任务域偷窃(小鹏老师推荐的方式)
            tbb::this_task_arena::isolate([&](){
                tbb::parallel_for(tbb::blocked_range<size_t>(0,n),[&](tbb::blocked_range<size_t> h){
                    for(size_t j = h.begin(); j < h.end(); ++j){
                        a[i * n + j] = std::sin(i) * std::sin(j);
                    }
                });
            });
        }
    });
#else
    // 使用std::mutex可能会产生死锁
    // 因为TBB 在并行计算期间，某个线程执行玩自己的工作队列后，会尝试去取别的线程未完成的队列数据，可能导致mutex重复上锁
    // 所以使用 std::recursive_mutex 来避免死锁
    tbb::parallel_for((size_t)0,(size_t)n,[&](size_t i){
        // c++17后就不需要写这个模板参数了
        std::lock_guard lck(mtx);
        tbb::parallel_for((size_t)0,(size_t)n,[&](size_t j){
            a[i * n + j] = std::sin(i) * std::sin(j);
        });
    });
#endif

    TOCK(area);
    return 0;
}
