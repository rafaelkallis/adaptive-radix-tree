/**
 * @file art tests
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "doctest.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <random>
#include <string>
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
    REQUIRE_EQ(nullptr, trie.get(key));
    trie.set(key, dummy_value_1);
    REQUIRE_EQ(dummy_value_1, trie.get(key));
  }

  SUBCASE("insert into empty tree & replace") {
    key_type key = {'a', 'b', 'c'};
    trie.set(key, dummy_value_1);
    trie.set(key, dummy_value_2);
    REQUIRE_EQ(dummy_value_2, trie.get(key));
  }

  SUBCASE("insert value s.t. existing value is a prefix") {
    key_type prefix_key = {'a', 'b', 'c'};
    key_type key = {'a', 'b', 'c', 'd', 'e'};
    trie.set(prefix_key, dummy_value_1);
    trie.set(key, dummy_value_2);
    REQUIRE_EQ(dummy_value_1, trie.get(prefix_key));
    REQUIRE_EQ(dummy_value_2, trie.get(key));
  }

  SUBCASE("insert value s.t. new value is a prefix") {
    key_type prefix_key = {'a', 'b', 'c'};
    key_type key = {'a', 'b', 'c', 'd', 'e'};
    trie.set(key, dummy_value_1);
    trie.set(prefix_key, dummy_value_2);
    REQUIRE_EQ(dummy_value_1, trie.get(key));
    REQUIRE_EQ(dummy_value_2, trie.get(prefix_key));
  }

  SUBCASE("insert key s.t. it mismatches existing key") {
    key_type key1 = {'a', 'a', 'a', 'a', 'a'};
    key_type key2 = {'a', 'a', 'b', 'a', 'a'};
    trie.set(key1, dummy_value_1);
    trie.set(key2, dummy_value_2);
    REQUIRE_EQ(dummy_value_1, trie.get(key1));
    REQUIRE_EQ(dummy_value_2, trie.get(key2));
  }

  SUBCASE("monte carlo") {
    const int n = 1000;
    array<key_type, n> keys;
    array<int *, n> values;
    /* rng */
    random_device rd;
    mt19937 g(rd());
    for (int experiment = 0; experiment < 10; experiment += 1) {
      std::cout << "art experiment: #" << experiment << std::endl;
      for (int i = 0; i < n; i += 1) {
        std::string random_num_str = std::to_string(g());
        keys[i] = key_type(random_num_str.cbegin(), random_num_str.cend());
        values[i] = new int();
      }

      art<int> subject;

      for (int i = 0; i < n; i += 1) {
        subject.set(keys[i], values[i]);

        for (int j = 0; j < i; j += 1) {
          REQUIRE_EQ(values[j], subject.get(keys[j]));
        }
      }

      for (int i = 0; i < n; i += 1) {
        delete values[i];
      }
    }
  }

  delete dummy_value_1;
  delete dummy_value_2;
}
