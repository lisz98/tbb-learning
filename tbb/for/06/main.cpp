#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <tbb/parallel_for.h>
#include <thread>
#include <vector>

using namespace tbb;

class ApplyFoo {
  float *const my_a;

public:
  void operator()(const blocked_range<size_t> &r) const {
    std::cout << r.size() << std::endl;
    float *a = my_a;
    for (size_t i = r.begin(); i != r.end(); ++i) {
      // std::cout << i << std::endl;
    }
  }
  ApplyFoo(float a[]) : my_a(a) {}
};

int main() {
  const size_t n = 100;
  float a[n] = {1};
  //blocked_range的第三个参数决定划分区域的颗粒度，默认为1
  // parallel_for的第三个参数指定为simple_partitioner后会关闭自动分块
 // auto_partitioner & affinity_partitioner
  // auto_partitioner是 parallel_for的默认分割器
  parallel_for(blocked_range<size_t>(0, n, 8), ApplyFoo(a),
               simple_partitioner());
}
