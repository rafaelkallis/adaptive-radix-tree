/**
 * @file query microbenchmarks
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "picobench/picobench.hpp"
#include <functional>
#include <map>
#include <random>
#include <string>
#include <unordered_map>
#include <queue>

using namespace art;
using picobench::state;

PICOBENCH_SUITE("query");

static void art_query_sparse(state &s) {
  art<int> m;
  int v = 1;
  std::mt19937_64 g1(0);
  for (auto _ : s) {
    m.set(std::to_string(g1()), &v);
  }
  std::mt19937_64 g2(0);
  for (auto _ : s) {
    auto v = m.get(std::to_string(g2()));
  }
}
PICOBENCH(art_query_sparse)
    .iterations({1 << 10, 1 << 20});

static void red_black_query_sparse(state &s) {
  std::map<key_type, int> m;
  int v = 1;
  std::mt19937_64 g1(0);
  for (auto _ : s) {
    m[std::to_string(g1())] = v;
  }
  std::mt19937_64 g2(0);
  for (auto _ : s) {
    auto v = m[std::to_string(g2())];
  }
}
PICOBENCH(red_black_query_sparse)
    .iterations({1 << 10, 1 << 20});

static void hashmap_query_sparse(state &s) {
  std::unordered_map<key_type, int> m;
  int v = 1;
  std::mt19937_64 g1(0);
  for (auto _ : s) {
    m[std::to_string(g1())] = v;
  }
  std::mt19937_64 g2(0);
  for (auto _ : s) {
    auto v = m[std::to_string(g2())];
  }
}
PICOBENCH(hashmap_query_sparse)
    .iterations({1 << 10, 1 << 20});
