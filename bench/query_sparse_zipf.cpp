/**
 * @file query microbenchmarks
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

PICOBENCH_SUITE("query sparse zipf");

static void art_q_s_z(state &s) {
  art::art<int> m;
  hash<uint32_t> h;
  int v = 1;
  int *v_ptr = &v;
  fast_zipf rng1(1000000);
  for (auto i __attribute__((unused)) : s) {
    m.set(to_string(h(rng1())).c_str(), v_ptr);
  }
  fast_zipf rng2(1000000);
  for (auto i __attribute__((unused)) : s) {
    v_ptr = m.get(to_string(h(rng2())).c_str());
  }
}
PICOBENCH(art_q_s_z);

static void red_black_q_s_z(state &s) {
  map<string, int> m;
  hash<uint32_t> h;
  int v = 1;
  fast_zipf rng1(1000000);
  for (auto i __attribute__((unused)) : s) {
    m[to_string(h(rng1()))] = v;
  }
  fast_zipf rng2(1000000);
  for (auto i __attribute__((unused)) : s) {
    v = m[to_string(h(rng2()))];
  }
}
PICOBENCH(red_black_q_s_z);

static void hashmap_q_s_z(state &s) {
  unordered_map<string, int> m;
  hash<uint32_t> h;
  int v = 1;
  fast_zipf rng1(1000000);
  for (auto i __attribute__((unused)) : s) {
    m[to_string(h(rng1()))] = v;
  }
  fast_zipf rng2(1000000);
  for (auto i __attribute__((unused)) : s) {
    v = m[to_string(h(rng2()))];
  }
}
PICOBENCH(hashmap_q_s_z);
