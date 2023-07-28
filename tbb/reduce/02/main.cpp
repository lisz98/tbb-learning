#include <cmath>
#include <iostream>
#include <tbb/blocked_range.h>
#include <tbb/parallel_reduce.h>
#include <vector>

int main() {
  size_t n = 1 << 26;
  //使用reduce 函数进行归并计算
  // 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 
  // (1 + 2) + (3 + 4) (5 + 6)  (7 + 8)
  // (3 + 7)  + (11 + 15)
  // 10 + 26
  // 36
  float res = tbb::parallel_reduce(
      tbb::blocked_range<size_t>(0, n), (float)0,
      [&](tbb::blocked_range<size_t> r, float local_res) {
        for (size_t i = r.begin(); i < r.end(); ++i) {
          local_res += std::sin(i);
        }
      },
      [](float x, float y) { return x + y; });

  std::cout << res << std::endl;
}
