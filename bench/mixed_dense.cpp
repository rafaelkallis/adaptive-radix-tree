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

PICOBENCH_SUITE("mixed_dense");

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
  fast_zipf rng(n);

  art::art<int> m;
  string k;
  int *val;
  for (auto _ : s) {
    k = dataset[rng()];
    if (m.get(k.c_str(), k.length()) == nullptr) {
      m.set(k.c_str(), k.length(), &v);
    } else {
      m.del(k.c_str(), k.length());
    }
  }
}
/* PICOBENCH(art_mixed).iterations({1000000}); */

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
  fast_zipf rng(n);

  map<string, int *> m;
  string k;
  int *val;
  for (auto _ : s) {
    k = dataset[rng()];
    if (m[k] == nullptr) {
      m[k] = &v;
    } else {
      m.erase(m.find(k));
    }
  }
}
/* PICOBENCH(red_black_mixed).iterations({1000000}); */

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
  fast_zipf rng(n);

  unordered_map<string, int *> m;
  string k;
  int *val;
  for (auto _ : s) {
    k = dataset[rng()];
    if (m[k] == nullptr) {
      m[k] = &v;
    } else {
      m.erase(m.find(k));
    }
  }
}
/* PICOBENCH(hashmap_mixed).iterations({1000000}); */
