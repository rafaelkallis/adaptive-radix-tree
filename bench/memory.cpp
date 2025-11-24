/**
 * @file memory usage benchmarks
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "zipf.hpp"
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

using std::string;
using std::to_string;
using std::hash;
using std::map;
using std::unordered_map;

// Number of elements to insert for memory benchmarks
const uint32_t NUM_ELEMENTS = 1000000;

/**
 * Memory benchmark with uniformly distributed keys
 * Uses nullptr values to measure only data structure overhead
 */
static void memory_uniform() {
  art::art<int*> m;
  hash<uint32_t> h;
  
  // Fill tree with nullptr values
  for (uint32_t i = 0; i < NUM_ELEMENTS; i++) {
    m.set(to_string(h(i)).c_str(), nullptr);
  }
  
  std::cout << "Inserted " << NUM_ELEMENTS << " elements (uniform distribution)" << std::endl;
}

/**
 * Memory benchmark with Zipfian distributed keys
 * Uses nullptr values to measure only data structure overhead
 */
static void memory_zipf() {
  art::art<int*> m;
  hash<uint32_t> h;
  fast_zipf rng(NUM_ELEMENTS);
  
  // Fill tree with nullptr values
  for (uint32_t i = 0; i < NUM_ELEMENTS; i++) {
    m.set(to_string(h(rng())).c_str(), nullptr);
  }
  
  std::cout << "Inserted " << NUM_ELEMENTS << " elements (zipfian distribution)" << std::endl;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <uniform|zipf>" << std::endl;
    return 1;
  }
  
  std::string mode(argv[1]);
  
  if (mode == "uniform") {
    memory_uniform();
  } else if (mode == "zipf") {
    memory_zipf();
  } else {
    std::cerr << "Unknown mode: " << mode << std::endl;
    std::cerr << "Valid modes: uniform, zipf" << std::endl;
    return 1;
  }
  
  return 0;
}
