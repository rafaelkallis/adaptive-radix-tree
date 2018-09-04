/**
 * @file insertion microbenchmarks
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

using picobench::state;

PICOBENCH_SUITE("insert");

static void art_insert_sparse(state &s) {
  art::art<int> m;
  int v = 1;
  std::mt19937_64 rng(0);
  for (auto _ : s) {
    m.set(std::to_string(rng()).c_str(), &v);
  }
}
PICOBENCH(art_insert_sparse);

static void red_black_insert_sparse(state &s) {
  std::map<std::string, int> m;
  int v = 1;
  std::mt19937_64 rng(0);
  for (auto _ : s) {
    m[std::to_string(rng())] = v;
  }
}
PICOBENCH(red_black_insert_sparse);

static void hashmap_insert_sparse(state &s) {
  std::unordered_map<std::string, int> m;
  int v = 1;
  std::random_device rd;
  std::mt19937_64 g(rd());
  for (auto _ : s) {
    m[std::to_string(g())] = v;
  }
}
PICOBENCH(hashmap_insert_sparse);
