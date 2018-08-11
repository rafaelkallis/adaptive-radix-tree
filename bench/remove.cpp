/**
 * @file removal microbenchmarks
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

PICOBENCH_SUITE("remove");

static void art_delete_sparse(state &s) {
  art<int> m;
  int v = 1;
  std::queue<std::string> q;
  std::random_device rd;
  std::mt19937_64 g(rd());
  for (auto _ : s) {
    auto key = std::to_string(g());
    m.set(key, &v);
    q.push(key);
  }
  for (auto _ : s) {
    auto key = q.front();
    q.pop();
    m.del(key);
  }
}
PICOBENCH(art_delete_sparse)
    .iterations({1 << 10, 1 << 20});

static void red_black_delete_sparse(state &s) {
  std::map<key_type, int> m;
  int v = 1;
  std::queue<std::string> q;
  std::random_device rd;
  std::mt19937_64 g(rd());
  for (auto _ : s) {
    auto key = std::to_string(g());
    m[key] = v;
    q.push(key);
  }
  for (auto _ : s) {
    auto key = q.front();
    q.pop();
    m.erase(m.find(key));
  }
}
PICOBENCH(red_black_delete_sparse)
    .iterations({1 << 10, 1 << 20});

static void hashmap_delete_sparse(state &s) {
  std::unordered_map<key_type, int> m;
  int v = 1;
  std::queue<std::string> q;
  std::random_device rd;
  std::mt19937_64 g(rd());
  for (auto _ : s) {
    auto key = std::to_string(g());
    m[key] = v;
    q.push(key);
  }
  for (auto _ : s) {
    auto key = q.front();
    q.pop();
    m.erase(m.find(key));
  }
}
PICOBENCH(hashmap_delete_sparse)
    .iterations({1 << 10, 1 << 20});
