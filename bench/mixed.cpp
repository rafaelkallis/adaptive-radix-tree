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

using namespace art;
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

static void art_mixed(state &s) {
  ifstream file("dataset.txt");
  if (!file) {
    return;
  }
  unordered_map<uint32_t, string> dataset;
  uint32_t n = 0;
  string line;
  while (getline(file, line)) {
    dataset[n++] = line;
  }
  file.close();

  int v = 1;
  fast_zipf rng(n, 1.0, 0);

  art<int> m;
  for (auto _ : s) {
    string k = dataset[rng()];
    int *val = m.get(k);
    if (val == nullptr) {
      m.set(k, &v);
    } else {
      m.del(k);
    }
  }
}
/* PICOBENCH(art_mixed).iterations({1 << 20}); */

static void red_black_mixed(state &s) {
  ifstream file("dataset.txt");
  if (!file) {
    return;
  }
  unordered_map<uint32_t, string> dataset;
  uint32_t n = 0;
  string line;
  while (getline(file, line)) {
    dataset[n++] = line;
  }
  file.close();

  int v = 1;
  fast_zipf rng(n, 1.0, 0);

  map<string, int *> m;
  for (auto _ : s) {
    string k = dataset[rng()];
    int *val = m[k];
    if (val == nullptr) {
      m[k] = &v;
    } else {
      m.erase(m.find(k));
    }
  }
}
/* PICOBENCH(red_black_mixed).iterations({1 << 20}); */

static void hashmap_mixed(state &s) {
  ifstream file("dataset.txt");
  if (!file) {
    return;
  }
  unordered_map<uint32_t, string> dataset;
  uint32_t n = 0;
  string line;
  while (getline(file, line)) {
    dataset[n++] = line;
  }
  file.close();

  int v = 1;
  fast_zipf rng(n, 1.0, 0);

  unordered_map<string, int *> m;
  for (auto _ : s) {
    string k = dataset[rng()];
    int *val = m[k];
    if (val == nullptr) {
      m[k] = &v;
    } else {
      m.erase(m.find(k));
    }
  }
}
/* PICOBENCH(hashmap_mixed).iterations({1 << 20}); */

static void art_mixed_sparse(state &s) {
  art<int> m;
  fast_zipf rng(1000000);
  hash<uint32_t> h;
  int v = 1;
  for (auto _ : s) {
    auto k = to_string(h(rng()));
    auto r = m.get(k);
    if (r == nullptr) {
      m.set(k, &v);
    } else {
      m.del(k);
    }
  }
}
PICOBENCH(art_mixed_sparse);

static void red_black_mixed_sparse(state &s) {
  map<string, int *> m;
  fast_zipf rng(1000000);
  hash<uint32_t> h;
  int v = 1;
  for (auto _ : s) {
    auto k = to_string(h(rng()));
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
  fast_zipf rng(1000000);
  hash<uint32_t> h;
  int v = 1;
  for (auto _ : s) {
    auto k = to_string(h(rng()));
    if (m[k] == nullptr) {
      m[k] = &v;
    } else {
      m.erase(m.find(k));
    }
  }
}
PICOBENCH(hashmap_mixed_sparse);
