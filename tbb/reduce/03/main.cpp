#include <cmath>
#include <iostream>
#include <tbb/blocked_range.h>
#include <tbb/parallel_reduce.h>
#include <vector>

int main() {
  size_t n = 1 << 26;
  // 确定性并行技术
  // 一般在并行编程中，难以进行debug,因为无法确定是 a 先与 b 发生，还是b 先与 a
  // 发生
#if 0
    //当使用 parallel_reduce时可以发现，计算结果每次都可能会出现不同
    float res = tbb::parallel_reduce(tbb::blocked_range<size_t>(0, n), (float)0,
#else
  // 虽然是并行的，但是确定性技术要求它的运行步骤和串行算法从表面上一致,这样就不会出现计算的中间变量每一次都不相同的情况，也可以避免出现
  // race，这样每次计算出来的值都是相同的
  float res = tbb::parallel_deterministic_reduce(
      tbb::blocked_range<size_t>(0, n), (float)0,
#endif
    [&] (tbb::blocked_range<size_t> r, float local_res) {
    for (size_t i = r.begin(); i < r.end(); i++) {
      local_res += std::sin(i);
    }
    return local_res;
    }, [] (float x, float y) {
    return x + y;
    });

    std::cout << res << std::endl;
    return 0;
}
