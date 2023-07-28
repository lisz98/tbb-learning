#include <cmath>
#include <iostream>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>
#include <vector>
#include "ticktok.h"

int main() {
  size_t n = 1 << 26;
  std::vector<float> a(n);

  // fill a with sin(i)
  TICK(for);
  tbb::parallel_for(tbb::blocked_range<size_t>(0, n),
                    [&](tbb::blocked_range<size_t> r) {
                      for (size_t i = r.begin(); i < r.end(); ++i) {
                        a[i] = std::sin(i);
                      }
                    });
  TOCK(for);



  TICK(reduce1);
  // calculate sum of a
  float res = 0;
  for (size_t i = 0; i < a.size(); i++) {
      res += a[i];
  }
  TOCK(reduce1); 
       // calculate sum of a
  TICK(reduce);
  res = tbb::parallel_reduce(
      tbb::blocked_range<size_t>(0, n),(float)0,
      [&](tbb::blocked_range<size_t> r, float local_res) {
        for (size_t i = r.begin(); i < r.end(); ++i) {
          local_res += a[i];
        }
        return local_res;
      },
      [](float x, float y) { return x + y; });
  TOCK(reduce);

  std::cout << res << std::endl;
  return 0;
}
