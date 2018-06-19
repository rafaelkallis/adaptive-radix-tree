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

TEST_SUITE("art") {

  art<int> trie;

  int dummy_value_1;
  int dummy_value_2;

  TEST_CASE("insert into empty tree") {
    key_type key = {'a', 'b', 'c'};
    REQUIRE_EQ(nullptr, trie.get(key));
    trie.set(key, &dummy_value_1);
    REQUIRE_EQ(&dummy_value_1, trie.get(key));
  }

  TEST_CASE("insert into empty tree & replace") {
    key_type key = {'a', 'b', 'c'};
    trie.set(key, &dummy_value_1);
    trie.set(key, &dummy_value_2);
    REQUIRE_EQ(&dummy_value_2, trie.get(key));
  }

  TEST_CASE("insert value s.t. existing value is a prefix") {
    key_type prefix_key = {'a', 'b', 'c'};
    key_type key = {'a', 'b', 'c', 'd', 'e'};
    trie.set(prefix_key, &dummy_value_1);
    trie.set(key, &dummy_value_2);
    REQUIRE_EQ(&dummy_value_1, trie.get(prefix_key));
    REQUIRE_EQ(&dummy_value_2, trie.get(key));
  }

  TEST_CASE("insert value s.t. new value is a prefix") {
    key_type prefix_key = {'a', 'b', 'c'};
    key_type key = {'a', 'b', 'c', 'd', 'e'};
    trie.set(key, &dummy_value_1);
    trie.set(prefix_key, &dummy_value_2);
    REQUIRE_EQ(&dummy_value_1, trie.get(key));
    REQUIRE_EQ(&dummy_value_2, trie.get(prefix_key));
  }

  TEST_CASE("insert key s.t. it mismatches existing key") {
    key_type key1 = {'a', 'a', 'a', 'a', 'a'};
    key_type key2 = {'a', 'a', 'b', 'a', 'a'};
    trie.set(key1, &dummy_value_1);
    trie.set(key2, &dummy_value_2);
    REQUIRE_EQ(&dummy_value_1, trie.get(key1));
    REQUIRE_EQ(&dummy_value_2, trie.get(key2));
  }

  TEST_CASE("monte carlo") {
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

  TEST_CASE("preorder traversal") {
    int int0;
    int int1;
    int int2;
    int int3;
    int int4;
    int int5;
    int int6;

    art<int> subject;

    subject.set(key_type{'a','a'}, &int0);
    subject.set(key_type{'a','a','a','a'}, &int1);
    subject.set(key_type{'a','a','a','a','a','a','a'}, &int2);
    subject.set(key_type{'a','a','a','a','a','a','a','a','a','a'}, &int3);
    subject.set(key_type{'a','a','a','a','a','a','a','b','a'}, &int4);
    subject.set(key_type{'a','a','a','a','b','a','a'}, &int5);
    subject.set(key_type{'a','a','a','a','b','a','a','a','a','a'}, &int6);

    /* The above statements construct the following tree:
     *
     *                (aa)->0
     *                 |a
     *                 |
     *                (a)->1
     *             a /   \ b
     *              /     \
     *        2<-(aa)     (aa)->5
     *         a /  \ b    |a
     *          /    \     |
     *    3<-(aa) 4<-(a)  (aa)->6
     */ 

    auto it = subject.preorder_traversal_begin();
    auto it_end = subject.preorder_traversal_end();

    // 0
    REQUIRE(it != it_end);
    REQUIRE_EQ(&int0, (*it)->get_value());

    ++it;
    // 1
    REQUIRE(it != it_end);
    REQUIRE_EQ(&int1, (*it)->get_value());
    
    ++it;
    // 2
    REQUIRE(it != it_end);
    REQUIRE_EQ(&int2, (*it)->get_value());
    
    ++it;
    // 3
    REQUIRE(it != it_end);
    REQUIRE_EQ(&int3, (*it)->get_value());
    
    ++it;
    // 4
    REQUIRE(it != it_end);
    REQUIRE_EQ(&int4, (*it)->get_value());
    
    ++it;
    // 5
    REQUIRE(it != it_end);
    REQUIRE_EQ(&int5, (*it)->get_value());
    
    ++it;
    // 6
    REQUIRE(it != it_end);
    REQUIRE_EQ(&int6, (*it)->get_value());

    ++it;
    // 7 (overflow)
    REQUIRE(it == it_end);
  }
}
