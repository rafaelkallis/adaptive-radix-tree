/**
 * @file art benchmarks
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "picobench/picobench.hpp"
#include <functional>
#include <map>
#include <random>
#include <string>
#include <unordered_map>

using namespace art;
using picobench::state;

PICOBENCH_SUITE("art");

static void art_insert_sparse(state &s) {
  art<int> tree;
  int v = 1;
  std::random_device rd;
  std::mt19937_64 g(rd());
  for (auto _ : s) {
    uint64_t raw_key = g();
    uint8_t *key_ptr = (uint8_t *)&raw_key;
    tree.set(key_type({
                 *key_ptr,
                 *(key_ptr + 1),
                 *(key_ptr + 2),
                 *(key_ptr + 3),
                 *(key_ptr + 4),
                 *(key_ptr + 5),
                 *(key_ptr + 6),
                 *(key_ptr + 7),
             }),
             &v);
  }
}
PICOBENCH(art_insert_sparse)
    .iterations({1 << 8, 1 << 10, 1 << 12, 1 << 14, 1 << 16, 1 << 18, 1 << 20});

static void std_map_insert_sparse(state &s) {
  std::map<key_type, int> std_map;
  int v = 1;
  std::random_device rd;
  std::mt19937_64 g(rd());
  for (auto _ : s) {
    uint64_t raw_key = g();
    uint8_t *key_ptr = (uint8_t *)&raw_key;
    std_map[key_type({
        *key_ptr,
        *(key_ptr + 1),
        *(key_ptr + 2),
        *(key_ptr + 3),
        *(key_ptr + 4),
        *(key_ptr + 5),
        *(key_ptr + 6),
        *(key_ptr + 7),
    })] = v;
  }
}
PICOBENCH(std_map_insert_sparse)
    .iterations({1 << 8, 1 << 10, 1 << 12, 1 << 14, 1 << 16, 1 << 18, 1 << 20});

// TODO(rafaelkallis): hash function
/* static void std_unordered_map_insert_sparse(state &s) { */
/*   std::unordered_map<key_type, int> std_unordered_map; */
/*   int v = 1; */
/*   std::random_device rd; */
/*   std::mt19937_64 g(rd()); */
/*   for (auto _ : s) { */
/*     uint64_t raw_key = g(); */
/*     uint8_t *key_ptr = (uint8_t *)&raw_key; */
/*     std_unordered_map[key_type({ */
/*         *key_ptr, */
/*         *(key_ptr + 1), */
/*         *(key_ptr + 2), */
/*         *(key_ptr + 3), */
/*         *(key_ptr + 4), */
/*         *(key_ptr + 5), */
/*         *(key_ptr + 6), */
/*         *(key_ptr + 7), */
/*     })] = v; */
/*   } */
/* } */
/* PICOBENCH(std_unordered_map_insert_sparse) */
/*     .iterations({1 << 8, 1 << 10, 1 << 12, 1 << 14, 1 << 16, 1 << 18, 1 <<
 * 20}); */
