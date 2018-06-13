/**
 * @file art tests
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

TEST_CASE("art") {

  art<int> trie;

  key_type key = {'a', 'b', 'c'};

  REQUIRE_EQ(nullptr, trie.search(key));

  int dummy_value = 1;
  int *dummy_value_ptr = &dummy_value;

  SUBCASE("insert into empty tree") { 
    trie.set(key, dummy_value_ptr); 
    REQUIRE_EQ(dummy_value_ptr, trie.search(key));
  }
}
