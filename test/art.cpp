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

  int *dummy_value_1 = new int();
  int *dummy_value_2 = new int();

  SUBCASE("insert into empty tree") {
    key_type key = {'a', 'b', 'c'};
    REQUIRE_EQ(nullptr, trie.search(key));
    trie.set(key, dummy_value_1);
    REQUIRE_EQ(dummy_value_1, trie.search(key));
  }

  SUBCASE("insert into empty tree & replace") {
    key_type key = {'a', 'b', 'c'};
    trie.set(key, dummy_value_1);
    trie.set(key, dummy_value_2);
    REQUIRE_EQ(dummy_value_2, trie.search(key));
  }

  SUBCASE("insert value s.t. existing value is a prefix") {
    key_type prefix_key = {'a', 'b', 'c'};
    key_type key = {'a', 'b', 'c', 'd', 'e'};
    trie.set(prefix_key, dummy_value_1);
    trie.set(key, dummy_value_2);
    REQUIRE_EQ(dummy_value_1, trie.search(prefix_key));
    REQUIRE_EQ(dummy_value_2, trie.search(key));
  }
  
  SUBCASE("insert value s.t. new value is a prefix") {
    key_type prefix_key = {'a', 'b', 'c'};
    key_type key = {'a', 'b', 'c', 'd', 'e'};
    trie.set(key, dummy_value_1);
    trie.set(prefix_key, dummy_value_2);
    REQUIRE_EQ(dummy_value_1, trie.search(key));
    REQUIRE_EQ(dummy_value_2, trie.search(prefix_key));
  }

  delete dummy_value_1;
  delete dummy_value_2;
}
