#ifndef ZIPF_HPP_
#define ZIPF_HPP_

#include <cmath>
#include <cstdint>
#include <functional>
#include <random>

class zipf {
public:
  zipf(uint32_t size, double skew, uint32_t seed = 0);

  uint32_t next();
  inline uint32_t operator()() { return next(); };

protected:
  const uint32_t size_;
  const double skew_;
  double denominator_;
  std::function<double()> rnd_;

  inline double probability_of(int rank) {
    return (1.0 / std::pow(rank, skew_)) / denominator_;
  }
};

class fast_zipf : public zipf {
public:
  fast_zipf(uint32_t size, double skew, uint32_t seed = 0);

  ~fast_zipf();
  fast_zipf(const fast_zipf &that) = delete;
  fast_zipf &operator=(const fast_zipf &that) = delete;

  uint32_t next();
  inline uint32_t operator()() { return next(); };

private:
  double *cdf_values_;

  void materialize_cdf();
  uint32_t binary_search_in_cdf(double key);
};

zipf::zipf(uint32_t size, double skew, uint32_t seed)
    : size_(size), skew_(skew) {
  denominator_ = 0;
  for (uint32_t rank = 1; rank <= size; ++rank) {
    denominator_ += (1.0 / std::pow(rank, skew_));
  }
  rnd_ = std::bind(std::uniform_real_distribution<double>(0.0, 1.0),
                   std::mt19937(seed));
}

uint32_t zipf::next() {
  double prob = rnd_();
  double cum_prob = 0.0;
  uint32_t rank = 0;
  while (cum_prob < prob) {
    ++rank;
    cum_prob += probability_of(rank);
  }
  return rank - 1;
}

fast_zipf::fast_zipf(uint32_t size, double skew, uint32_t seed)
    : zipf(size, skew, seed) {
  cdf_values_ = new double[size_];
  materialize_cdf();
}

fast_zipf::~fast_zipf() { delete[] cdf_values_; }

void fast_zipf::materialize_cdf() {
  double cdf = 0;
  for (uint32_t rank = 1; rank <= size_; ++rank) {
    cdf += probability_of(rank);
    cdf_values_[rank - 1] = cdf;
  }
}

uint32_t fast_zipf::next() { return binary_search_in_cdf(rnd_()); }

uint32_t fast_zipf::binary_search_in_cdf(double key) {
  int lo = 0;
  int hi = size_ - 1;
  while (lo <= hi) {
    int mid = lo + (hi - lo) / 2;
    if (cdf_values_[mid] == key) {
      return mid;
    } else if (cdf_values_[mid] < key) {
      lo = mid + 1;
    } else {
      hi = mid - 1;
    }
  }
  return lo;
}

#endif // ZIPF_HPP_
