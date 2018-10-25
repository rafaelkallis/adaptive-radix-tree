/**
 * @file mixed benchmarks
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "picobench/picobench.hpp"
#include "zipf.hpp"
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <queue>
#include <random>
#include <string>
#include <unordered_map>

using picobench::state;
using std::getline;
using std::hash;
using std::ifstream;
using std::map;
using std::mt19937_64;
using std::string;
using std::to_string;
using std::unordered_map;

PICOBENCH_SUITE("mixed");

static void art_mixed_sparse(state &s) {
  art::art<int> m;
  fast_zipf rng(10000000);
  hash<uint32_t> h;
  string k;
  int v = 1;
  for (auto _ : s) {
    k = to_string(h(rng()));
    if (m.get(k.c_str()) == nullptr) {
      m.set(k.c_str(), &v);
    } else {
      m.del(k.c_str());
    }
  }
}
PICOBENCH(art_mixed_sparse);

static void red_black_mixed_sparse(state &s) {
  map<string, int *> m;
  fast_zipf rng(10000000);
  hash<uint32_t> h;
  string k;
  int v = 1;
  for (auto _ : s) {
    k = to_string(h(rng()));
    if (m[k] == nullptr) {
      m[k] = &v;
    } else {
      m.erase(m.find(k));
    }
  }
}
PICOBENCH(red_black_mixed_sparse);

static void hashmap_mixed_sparse(state &s) {
  unordered_map<string, int *> m;
  fast_zipf rng(10000000);
  hash<uint32_t> h;
  string k;
  int v = 1;
  for (auto _ : s) {
    k = to_string(h(rng()));
    if (m[k] == nullptr) {
      m[k] = &v;
    } else {
      m.erase(m.find(k));
    }
  }
}
PICOBENCH(hashmap_mixed_sparse);
