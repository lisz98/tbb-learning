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
#else
  // 计算平均数
  std::vector<float> a(n);
  for (size_t i = 0; i < n; i++) {
    // 我们知道 sin 数相加会无限接近于0,正确的平均数应该是趋近于10的
    a[i] = 10.f + std::sin(i);
  }

  float serial_avg = 0;
  for (size_t i = 0; i < n; ++i) {
    // 使用串行计算平均数会出现当
    // serial_avg足够大时，例如10.0,再加上一个足够小的数0.0000001时会出现精度丢失，而忽略掉0.0000001这个数
    // 所以计算出来的结果极有可能不正确
    serial_avg += a[i];
  }

  serial_avg /= n;
  //计算出来的结果为4，计算错误
  std::cout << serial_avg << std::endl;

  // 在计算平均数的过程中，使用归并算法计算的平均数正确率高
  float parallel_avg = tbb::parallel_reduce(
      tbb::blocked_range<size_t>(0, n), (float)0,
      [&](tbb::blocked_range<size_t> r, float local_avg) {
        for (size_t i = r.begin(); i < r.end(); ++i) {
          local_avg += a[i];
        }
        return local_avg;
      },
      [](float a, float b) { return a + b; });

  parallel_avg /= n;
  std::cout << parallel_avg << std::endl;
#endif
  return 0;
}
