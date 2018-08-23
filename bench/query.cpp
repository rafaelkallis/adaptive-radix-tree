/**
 * @file query microbenchmarks
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "picobench/picobench.hpp"
#include "zipf.hpp"
#include <functional>
#include <map>
#include <queue>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>

using namespace art;
using picobench::state;
using std::string;
using std::to_string;
using std::hash;
using std::mt19937_64;
using std::map;
using std::unordered_map;

PICOBENCH_SUITE("query");

static void art_query_sparse(state &s) {
  art<int> m;
  hash<uint32_t> h;
  int v = 1;
  mt19937_64 rng1(0);
  for (auto _ : s) {
    m.set(to_string(h(rng1())), &v);
  }
  mt19937_64 rng2(0);
  for (auto _ : s) {
    m.get(to_string(h(rng2())));
  }
}
PICOBENCH(art_query_sparse);

static void art_query_sparse_zipf(state &s) {
  art<int> m;
  hash<uint32_t> h;
  int v = 1;
  fast_zipf rng1(1000000);
  for (auto _ : s) {
    m.set(to_string(h(rng1())), &v);
  }
  fast_zipf rng2(1000000);
  for (auto _ : s) {
    m.get(to_string(h(rng2())));
  }
}
PICOBENCH(art_query_sparse_zipf);

static void red_black_query_sparse(state &s) {
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
PICOBENCH(red_black_query_sparse);

static void red_black_query_sparse_zipf(state &s) {
  map<string, int> m;
  hash<uint32_t> h;
  int v = 1;
  fast_zipf rng1(1000000);
  for (auto _ : s) {
    m[to_string(h(rng1()))] = v;
  }
  fast_zipf rng2(1000000);
  for (auto _ : s) {
    auto v = m[to_string(h(rng2()))];
  }
}
PICOBENCH(red_black_query_sparse_zipf);

static void hashmap_query_sparse(state &s) {
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
PICOBENCH(hashmap_query_sparse);

static void hashmap_query_sparse_zipf(state &s) {
  unordered_map<string, int> m;
  hash<uint32_t> h;
  int v = 1;
  fast_zipf rng1(1000000);
  for (auto _ : s) {
    m[to_string(h(rng1()))] = v;
  }
  fast_zipf rng2(1000000);
  for (auto _ : s) {
    auto v = m[to_string(h(rng2()))];
  }
}
PICOBENCH(hashmap_query_sparse_zipf);
