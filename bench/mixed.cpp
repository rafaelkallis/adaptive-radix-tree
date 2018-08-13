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

PICOBENCH_SUITE("mixed");

static void art_mixed(state &s) {
  std::ifstream file("dataset.txt");
  if (!file) {
    return;
  }
  std::unordered_map<uint32_t, std::string> dataset;
  uint32_t n = 0;
  std::string line;
  while (std::getline(file, line)) {
    dataset[n++] = line;
  }
  file.close();

  fast_zipf rng(n, 1.0, 0);

  art<int> m;
  int v = 1;
  for (auto _ : s) {
    std::string k = dataset[rng()];
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
  std::ifstream file("dataset.txt");
  if (!file) {
    return;
  }
  std::unordered_map<uint32_t, std::string> dataset;
  uint32_t n = 0;
  std::string line;
  while (std::getline(file, line)) {
    dataset[n++] = line;
  }
  file.close();

  fast_zipf rng(n, 1.0, 0);

  std::map<key_type, int *> m;
  int v = 1;
  for (auto _ : s) {
    std::string k = dataset[rng()];
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
  std::ifstream file("dataset.txt");
  if (!file) {
    return;
  }
  std::unordered_map<uint32_t, std::string> dataset;
  uint32_t n = 0;
  std::string line;
  while (std::getline(file, line)) {
    dataset[n++] = line;
  }
  file.close();

  fast_zipf rng(n, 1.0, 0);

  std::unordered_map<key_type, int *> m;
  int v = 1;
  for (auto _ : s) {
    std::string k = dataset[rng()];
    int *val = m[k];
    if (val == nullptr) {
      m[k] = &v;
    } else {
      m.erase(m.find(k));
    }
  }
}
/* PICOBENCH(hashmap_mixed).iterations({1 << 20}); */
