
#ifndef ZIPF_HPP
#define ZIPF_HPP

#include <cmath>
#include <cstdint>
#include <functional>
#include <random>

class zipf {
public:
  explicit zipf(uint32_t size, double skew = 1.0, uint32_t seed = 0);

  uint32_t operator()();

protected:
  const uint32_t size_;
  const double skew_;
  double denominator_;
  std::function<double()> rnd_;

  double probability_of(int rank);
};

class fast_zipf : public zipf {
public:
  explicit fast_zipf(uint32_t size, double skew = 1.0, uint32_t seed = 0);
  ~fast_zipf();

  uint32_t operator()();

private:
  double *cdf_values_;

  void materialize_cdf();
  uint32_t binary_search_in_cdf(double key);
};

#endif
