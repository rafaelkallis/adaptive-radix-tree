/**
 * @file node_256 tests
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

TEST_CASE("node_256") {
  /* set up */
  array<partial_key_type, 256> partial_keys;
  array<node_0<void> *, 256> children;

  for (int i = 0; i < 256; i += 1) {
    /* populate partial_keys with all values in the partial_keys_t domain */
    partial_keys[i] = i;

    /* populate child nodes */
    children[i] = new node_0<void>();
  }

  /* rng */
  random_device rd;
  mt19937 g(rd());

  for (int experiment = 0; experiment < 100; experiment += 1) {
    /* test subject */
    node_256<void> node;

    /* shuffle in order to make a seemingly random insertion order */
    shuffle(partial_keys.begin(), partial_keys.end(), g);
    for (int i = 0; i < 256; i += 1) {
      REQUIRE_FALSE(node.is_full());

      auto partial_key = partial_keys[i];
      auto child = children[partial_key];
      node.set_child(partial_key, child);

      for (int j = 0; j <= i; j += 1) {
        auto p_k = partial_keys[j];
        auto expected_child = children[p_k];
        auto actual_child_ptr = node.find_child(p_k);
        REQUIRE(actual_child_ptr != nullptr);
        auto actual_child = *actual_child_ptr;
        REQUIRE_EQ(expected_child, actual_child);
      }
    }
    REQUIRE(node.is_full());
  }

  /* tear down */
  for (int i = 0; i < 256; i += 1) {
    delete children[i];
  }
}
