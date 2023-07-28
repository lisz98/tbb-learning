#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <tbb/parallel_for.h>
#include <tbb/parallel_for_each.h>
#include <thread>
#include <vector>
#include <chrono>
#include <tbb/task_group.h>
struct Timer
{
    using TP = std::chrono::time_point<std::chrono::steady_clock,std::chrono::duration<double,std::milli>>;
    Timer(){
        start  =std::chrono::steady_clock::now();
    }
    ~Timer(){
        end = std::chrono::steady_clock::now();
        std::cout << "time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds> (end -start).count() <<"ms"<< std::endl;
    }

    TP start,end;
};

#define for_time(x) { \
    tbb::task_group tg;\
    Timer ____t; \
    tg.run([&](){ \
        x;\
    });\
    tg.wait();\
}

int main() {
  size_t n = 1 << 26;
  std::vector<float> a(n);

  // 将区间为(0,n]的数分为4份，分别使用一个线程去进行计算
#if 0
  for_time(tbb::parallel_for(tbb::blocked_range<size_t>(0, n),
                    [&](tbb::blocked_range<size_t> r) {
                      for (size_t i = r.begin(); i < r.end(); i++) {
                        a[i] = std::sin(i);
                      }
                    }));
#endif 
#if 0
  //这个版本比上面的版本简单，但是编译器无法进行SMID优化
  for_time(tbb::parallel_for((size_t)0,(size_t)n,[&](size_t i){
      a[i] = std::sin(i);
  }));
#endif 

#if 1
  //这个版本可以直接使用容器的迭代器进行操作
  for_time(tbb::parallel_for_each(a.begin(),a.end(),[&](float &f){
      f =32.f;
  }));
  for_time(tbb::parallel_for_each(a.begin(),a.end(),[&](float &f){
      std::cout << f << std::endl;
  }));
#endif
  return 0;
}
