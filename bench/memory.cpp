/**
 * @file memory benchmark
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "zipf.hpp"
#include <functional>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

using std::string;
using std::to_string;
using std::hash;
using std::mt19937_64;
using std::cout;
using std::cerr;
using std::endl;

// Zipfian distribution range (number of unique keys in distribution)
const size_t ZIPF_RANGE = 1000000;

/**
 * Benchmark memory usage with uniformly distributed keys
 */
void benchmark_uniform(size_t n_keys) {
  art::art<int*> m;
  hash<uint32_t> h;
  int *v_ptr = nullptr;
  mt19937_64 rng(0);
  
  cout << "Filling tree with " << n_keys << " uniformly distributed keys..." << endl;
  for (size_t i = 0; i < n_keys; ++i) {
    m.set(to_string(h(rng())).c_str(), v_ptr);
  }
  cout << "Tree filled." << endl;
  cout << "Press Enter to exit and generate massif report..." << endl;
  std::cin.get();
}

/**
 * Benchmark memory usage with zipfian distributed keys
 */
void benchmark_zipf(size_t n_keys) {
  art::art<int*> m;
  hash<uint32_t> h;
  int *v_ptr = nullptr;
  fast_zipf rng(ZIPF_RANGE);
  
  cout << "Filling tree with " << n_keys << " zipfian distributed keys..." << endl;
  for (size_t i = 0; i < n_keys; ++i) {
    m.set(to_string(h(rng())).c_str(), v_ptr);
  }
  cout << "Tree filled." << endl;
  cout << "Press Enter to exit and generate massif report..." << endl;
  std::cin.get();
}

int main(int argc, char *argv[]) {
  size_t n_keys = 100000;
  
  if (argc > 1) {
    try {
      n_keys = std::stoul(argv[1]);
    } catch (const std::invalid_argument& e) {
      cerr << "Error: Invalid number of keys: " << argv[1] << endl;
      cerr << "Usage: " << argv[0] << " [n_keys] [uniform|zipf]" << endl;
      return 1;
    } catch (const std::out_of_range& e) {
      cerr << "Error: Number of keys out of range: " << argv[1] << endl;
      cerr << "Usage: " << argv[0] << " [n_keys] [uniform|zipf]" << endl;
      return 1;
    }
  }
  
  string distribution = "uniform";
  if (argc > 2) {
    distribution = argv[2];
  }
  
  cout << "Memory Benchmark - ART (Adaptive Radix Tree)" << endl;
  cout << "=============================================" << endl;
  cout << "Distribution: " << distribution << endl;
  cout << "Number of keys: " << n_keys << endl;
  cout << endl;
  
  if (distribution == "uniform") {
    benchmark_uniform(n_keys);
  } else if (distribution == "zipf" || distribution == "zipfian") {
    benchmark_zipf(n_keys);
  } else {
    cerr << "Unknown distribution: " << distribution << endl;
    cerr << "Usage: " << argv[0] << " [n_keys] [uniform|zipf]" << endl;
    return 1;
  }
  
  return 0;
}
