/**
 * @file query microbenchmarks
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "picobench/picobench.hpp"
#include <functional>
#include <map>
#include <queue>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>

using picobench::state;
using std::string;
using std::to_string;
using std::hash;
using std::mt19937_64;
using std::map;
using std::unordered_map;

PICOBENCH_SUITE("query sparse uniform");

static void art_q_s_u(state &s) {
  art::art<int> m;
  hash<uint32_t> h;
  int v = 1;
  mt19937_64 rng1(0);
  for (auto _ : s) {
    m.set(to_string(h(rng1())).c_str(), &v);
  }
  mt19937_64 rng2(0);
  for (auto _ : s) {
    m.get(to_string(h(rng2())).c_str());
  }
}
PICOBENCH(art_q_s_u)
  /* .iterations({4000000}) */
  ;

static void red_black_q_s_u(state &s) {
  map<string, int> m;
  hash<uint32_t> h;
  int v = 1;
  mt19937_64 rng1(0);
  for (auto _ : s) {
    m[to_string(h(rng1()))] = v;
  }
  mt19937_64 rng2(0);
  for (auto _ : s) {
    auto v = m[to_string(h(rng2()))];
  }
}
PICOBENCH(red_black_q_s_u)
  /* .iterations({4000000}) */
  ;

static void hashmap_q_s_u(state &s) {
  unordered_map<string, int> m;
  hash<uint32_t> h;
  int v = 1;
  mt19937_64 rng1(0);
  for (auto _ : s) {
    m[to_string(h(rng1()))] = v;
  }
  mt19937_64 rng2(0);
  for (auto _ : s) {
    auto v = m[to_string(h(rng2()))];
  }
}
PICOBENCH(hashmap_q_s_u)
  /* .iterations({4000000}) */
  ;
