#include <cmath>
#include <iostream>
#include <tbb/blocked_range.h>
#include <tbb/parallel_scan.h>
#include <tbb/task_group.h>
#include <vector>

int main() {
  size_t n = 1 << 26;
  std::vector<float> a(n);
#if 0
    //实现一个扫描算法
    //1 2 3 4
    // a[0] = 3 ,a[1] = 6, a[2] = 10
    float res = 0;

    size_t maxt = 4;
    //创建一个任务组
    tbb::task_group tg1;
    std::vector<float> tmp_res(maxt);
    for (size_t t = 0; t < maxt; t++) {
        size_t beg = t * n / maxt;
        size_t end = std::min(n, (t + 1) * n / maxt);
        //任务组中启动多个任务，将数组分块进行计算
        tg1.run([&, t, beg, end] {
            float local_res = 0;
            for (size_t i = beg; i < end; i++) {
                local_res += std::sin(i);
            }
            tmp_res[t] = local_res;
        });
    }
    tg1.wait();
    for (size_t t = 0; t < maxt; t++) {
        tmp_res[t] += res;
        res = tmp_res[t];
    }
    tbb::task_group tg2;
    for (size_t t = 1; t < maxt; t++) {
        size_t beg = t * n / maxt - 1;
        size_t end = std::min(n, (t + 1) * n / maxt) - 1;
        tg2.run([&, t, beg, end] {
            float local_res = tmp_res[t];
            for (size_t i = beg; i < end; i++) {
                local_res += std::sin(i);
                a[i] = local_res;
            }
        });
    }
    tg2.wait();

#else
  // tbb 提供的一种并行的扫描算法
  float res = tbb::parallel_scan(
      tbb::blocked_range<size_t>(0, n), (float)0,
      [&](tbb::blocked_range<size_t> r, float local_res, auto is_final) {
        for (int i = r.begin(); r.end(); ++i) {
          local_res += std::sin(i);
          // scan算法中会自动计算是否为正确的值
          if (is_final) {
            a[i] = local_res;
          }
          return local_res;
        }
      },
      [](float x, float y) { return x + y; });

#endif
  std::cout << a[n / 2] << std::endl;
  std::cout << res << std::endl;
  return 0;
}
