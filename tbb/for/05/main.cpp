#include <atomic>
#include <cmath>
#include <iostream>
#include <tbb/blocked_range3d.h>
#include <tbb/parallel_for.h>
#include <vector>

int main() {
  size_t n = 1<<11;
  std::vector<float> a(n *n *n);
#if 1
  // 进行二阶for运算
  tbb::parallel_for(
      tbb::blocked_range3d<size_t>(0, n, 0, n,0,n),
      [&](tbb::blocked_range3d<size_t> r) {
        for (size_t i = r.pages().begin(); i < r.pages().end(); i++) {
          for (size_t j = r.cols().begin(); j < r.cols().end(); ++j) {
            for (size_t k = r.rows().begin(); k < r.rows().end(); ++k) {
                a[(i * n +j) * n + k] = std::sin(i) *std::sin(j)*std::sin(k);
            }
          }
        }
      });
#else
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      ret += 1;
    }
  }
#endif

  return 0;
}
