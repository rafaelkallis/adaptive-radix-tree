/**
 * @file node_4 tests
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "doctest.h"
#include <algorithm>
#include <array>
#include <random>
#include <vector>

using namespace art;

using std::array;
using std::mt19937;
using std::random_device;
using std::shuffle;

TEST_CASE("node_4") {
  array<partial_key_type, 256> partial_keys;
  array<node_0<void> *, 256> partial_key_to_child;

  /* populate partial_keys with all values in the partial_keys_t domain */
  for (int i = 0; i < 256; i += 1) {
    partial_keys[i] = i;
  }

  /* shuffle in order to make a seemingly random insertion order */
  random_device rd;
  mt19937 g(rd());
  shuffle(partial_keys.begin(), partial_keys.end(), g);

  node_4<void> node;

  SUBCASE("child insertion") {
    for (int i = 0; i < 4; i += 1) {
      REQUIRE_FALSE(node.is_full());

      auto partial_key = partial_keys[i];
      auto child = partial_key_to_child[partial_key];
      node.set_child(partial_key, child);

      for (int j = 0; j <= i; j += 1) {
        auto p_k = partial_keys[j];
        auto c = partial_key_to_child[p_k];
        REQUIRE_EQ(c, node.find_child(p_k));
      }
    }
    REQUIRE(node.is_full());
  }
}
