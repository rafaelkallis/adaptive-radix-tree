/**
 * @file query iteration microbenchmarks
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "picobench/picobench.hpp"
#include "zipf.hpp"
#include <functional>
#include <map>
#include <string>
#include <unordered_map>

using picobench::state;
using std::string;
using std::to_string;
using std::hash;
using std::mt19937_64;
using std::map;
using std::unordered_map;

PICOBENCH_SUITE("query iteration");

static void full_scan_zipf(state &s) {
  art::art<int> m;
  hash<uint32_t> h;
  int v = 1;
  int *v_ptr = &v;
  art::tree_it<int> it, it_end;
  fast_zipf rng(1000000);
  for (int i = 0; i < 100000; ++i) {
    m.set(to_string(h(rng())).c_str(), v_ptr);
  }
  for (auto i __attribute__((unused)) : s) {
    for (it = m.begin(), it_end = m.end(); it != it_end; ++it) {}
  }
}
PICOBENCH(full_scan_zipf).iterations({1000});

static void full_scan_uniform(state &s) {
  art::art<int> m;
  hash<uint32_t> h;
  int v = 1;
  int *v_ptr = &v;
  art::tree_it<int> it, it_end;
  mt19937_64 rng(0);
  for (int i = 0; i < 100000; ++i) {
    m.set(to_string(h(rng())).c_str(), v_ptr);
  }
  for (auto i __attribute__((unused)) : s) {
    for (it = m.begin(), it_end = m.end(); it != it_end; ++it) {}
  }
}
PICOBENCH(full_scan_uniform).iterations({1000});