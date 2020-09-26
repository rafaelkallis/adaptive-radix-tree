/**
 * @file deletion microbenchmarks
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "picobench/picobench.hpp"
#include <map>
#include <random>
#include <string>
#include <unordered_map>

using picobench::state;

PICOBENCH_SUITE("delete");

static void art_delete_sparse(state &s) {
  art::art<int> m;
  int v = 1;
  std::mt19937_64 g1(0);
  for (auto i __attribute__((unused)) : s) {
    m.set(std::to_string(g1()).c_str(), &v);
  }
  std::mt19937_64 g2(0);
  for (auto i __attribute__((unused)) : s) {
    m.del(std::to_string(g2()).c_str());
  }
}
PICOBENCH(art_delete_sparse);

static void red_black_delete_sparse(state &s) {
  std::map<std::string, int> m;
  int v = 1;
  std::mt19937_64 g1(0);
  for (auto i __attribute__((unused)) : s) {
    m[std::to_string(g1()).c_str()] = v;
  }
  std::mt19937_64 g2(0);
  for (auto i __attribute__((unused)) : s) {
    m.erase(m.find(std::to_string(g2())));
  }
}
PICOBENCH(red_black_delete_sparse);

static void hashmap_delete_sparse(state &s) {
  std::unordered_map<std::string, int> m;
  int v = 1;
  std::mt19937_64 g1(0);
  for (auto i __attribute__((unused)) : s) {
    m[std::to_string(g1())] = v;
  }
  std::mt19937_64 g2(0);
  for (auto i __attribute__((unused)) : s) {
    m.erase(m.find(std::to_string(g2())));
  }
}
PICOBENCH(hashmap_delete_sparse);
