#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <tbb/parallel_for.h>
#include <thread>
#include <vector>

int main() {
  size_t n = 1 << 26;
  std::vector<float> a(n);

  size_t maxt = 4;
  // 将区间为(0,n]的数分为4份，分别使用一个线程去进行计算
  tbb::parallel_for(tbb::blocked_range<size_t>(0, n),
                    [&](tbb::blocked_range<size_t> r) {
                      for (size_t i = r.begin(); i < r.end(); i++) {
                        a[i] = std::sin(i);
                      }
                    });

  return 0;
}
