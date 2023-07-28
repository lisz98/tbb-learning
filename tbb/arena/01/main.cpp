#include <tbb/task_arena.h>
#include <iostream>
#include <tbb/parallel_for.h>
#include <vector>
#include <cmath>
#include <string>
#include "ticktock.h"

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
            fprintf(stdout,(const char*)buffer);
        }
    }
    fprintf(stdout,"\n");
    fclose(file);
    
    std::cout << "computer max concurrency is :" << max_concurrency << std::endl;
    
    return max_concurrency;
}

int main(){
    size_t n = 1 << 26;
    std::vector<float> a(n);
    auto max_concurrency = get_max_concurrency();
    tbb::task_arena area(max_concurrency == 1 ? 2
                                              : max_concurrency);
    TICK(area);
    area.execute([&](
            ){
        //std::cout << "current area id : " << tbb::this_task_arena::current_thread_index() << std::endl;
        tbb::parallel_for((size_t)0,(size_t)n,[&](size_t i){
            a[i] = std::sin(i);
        });
    });
    TOCK(area);
    return 0;
}
