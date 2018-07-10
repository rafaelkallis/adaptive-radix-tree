/**
 * @file art tests
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "doctest.h"
#include <algorithm>
#include <array>
#include <random>
#include <string>
#include <vector>

using namespace art;

using std::array;
using std::mt19937;
using std::random_device;
using std::shuffle;

TEST_SUITE("art") {

  TEST_CASE("set") {

    art<int> trie;

    int dummy_value_1;
    int dummy_value_2;

    SUBCASE("insert into empty tree") {
      key_type key = "abc";
      REQUIRE_EQ(nullptr, trie.get(key));
      trie.set(key, &dummy_value_1);
      REQUIRE_EQ(&dummy_value_1, trie.get(key));
    }

    SUBCASE("insert into empty tree & replace") {
      key_type key = "abc";
      trie.set(key, &dummy_value_1);
      trie.set(key, &dummy_value_2);
      REQUIRE_EQ(&dummy_value_2, trie.get(key));
    }

    SUBCASE("insert value s.t. existing value is a prefix") {
      key_type prefix_key = "abc";
      key_type key = {'a', 'b', 'c', 'd', 'e'};
      trie.set(prefix_key, &dummy_value_1);
      trie.set(key, &dummy_value_2);
      REQUIRE_EQ(&dummy_value_1, trie.get(prefix_key));
      REQUIRE_EQ(&dummy_value_2, trie.get(key));
    }

    SUBCASE("insert value s.t. new value is a prefix") {
      key_type prefix_key = {'a', 'b', 'c'};
      key_type key = {'a', 'b', 'c', 'd', 'e'};
      trie.set(key, &dummy_value_1);
      trie.set(prefix_key, &dummy_value_2);
      REQUIRE_EQ(&dummy_value_1, trie.get(key));
      REQUIRE_EQ(&dummy_value_2, trie.get(prefix_key));
    }

    SUBCASE("insert key s.t. it mismatches existing key") {
      key_type key1 = {'a', 'a', 'a', 'a', 'a'};
      key_type key2 = {'a', 'a', 'b', 'a', 'a'};
      trie.set(key1, &dummy_value_1);
      trie.set(key2, &dummy_value_2);
      REQUIRE_EQ(&dummy_value_1, trie.get(key1));
      REQUIRE_EQ(&dummy_value_2, trie.get(key2));
    }

    SUBCASE("monte carlo") {
      const int n = 1000;
      array<key_type, n> keys;
      array<int *, n> values;
      /* rng */
      random_device rd;
      mt19937 g(rd());
      for (int experiment = 0; experiment < 10; experiment += 1) {
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
  }

  TEST_CASE("delete value") {
  
    key_type key0 = "aa";
    int int0 = 0;
    key_type key1 = "aaaa";
    int int1 = 1;
    key_type key2 = "aaaaaaa";
    int int2 = 2;
    key_type key3 = "aaaaaaaaaa";
    int int3 = 3;
    key_type key4 = "aaaaaaaba";
    int int4 = 4;
    key_type key5 = "aaaabaa";
    int int5 = 5;
    key_type key6 = "aaaabaaaaa";
    int int6 = 6;

    art<int> subject;

    subject.set(key0, &int0);
    subject.set(key1, &int1);
    subject.set(key2, &int2);
    subject.set(key3, &int3);
    subject.set(key4, &int4);
    subject.set(key5, &int5);
    subject.set(key6, &int6);

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

    SUBCASE("delete non existing value") {
      REQUIRE(subject.del("aaaaa") == nullptr);
      REQUIRE(subject.del("aaaaaa") == nullptr);
      REQUIRE(subject.del("aaaab") == nullptr);
      REQUIRE(subject.del("aaaaba") == nullptr);
      REQUIRE(subject.get(key0) == &int0);
      REQUIRE(subject.get(key1) == &int1);
      REQUIRE(subject.get(key2) == &int2);
      REQUIRE(subject.get(key3) == &int3);
      REQUIRE(subject.get(key4) == &int4);
      REQUIRE(subject.get(key5) == &int5);
      REQUIRE(subject.get(key6) == &int6);
    }

    SUBCASE("delete leaf with no siblings (6)") {
      REQUIRE(subject.del(key6) == &int6);
      REQUIRE(subject.get(key0) == &int0);
      REQUIRE(subject.get(key1) == &int1);
      REQUIRE(subject.get(key2) == &int2);
      REQUIRE(subject.get(key3) == &int3);
      REQUIRE(subject.get(key4) == &int4);
      REQUIRE(subject.get(key5) == &int5);
      REQUIRE(subject.get(key6) == nullptr);
    }
    
    SUBCASE("delete leaf with sibling (3)") {
      REQUIRE(subject.del(key3) == &int3);
      REQUIRE(subject.get(key0) == &int0);
      REQUIRE(subject.get(key1) == &int1);
      REQUIRE(subject.get(key2) == &int2);
      REQUIRE(subject.get(key3) == nullptr);
      REQUIRE(subject.get(key4) == &int4);
      REQUIRE(subject.get(key5) == &int5);
      REQUIRE(subject.get(key6) == &int6);
    }
    
    SUBCASE("delete leaf with sibling (4)") {
      REQUIRE(subject.del(key4) == &int4);
      REQUIRE(subject.get(key0) == &int0);
      REQUIRE(subject.get(key1) == &int1);
      REQUIRE(subject.get(key2) == &int2);
      REQUIRE(subject.get(key3) == &int3);
      REQUIRE(subject.get(key4) == nullptr);
      REQUIRE(subject.get(key5) == &int5);
      REQUIRE(subject.get(key6) == &int6);
    }

    SUBCASE("delete internal node with >1 children (2),(1)") {
      REQUIRE(subject.del(key2) == &int2);
      REQUIRE(subject.get(key0) == &int0);
      REQUIRE(subject.get(key1) == &int1);
      REQUIRE(subject.get(key2) == nullptr);
      REQUIRE(subject.get(key3) == &int3);
      REQUIRE(subject.get(key4) == &int4);
      REQUIRE(subject.get(key5) == &int5);
      REQUIRE(subject.get(key6) == &int6);
      
      REQUIRE(subject.del(key1) == &int1);
      REQUIRE(subject.get(key0) == &int0);
      REQUIRE(subject.get(key1) == nullptr);
      REQUIRE(subject.get(key2) == nullptr);
      REQUIRE(subject.get(key3) == &int3);
      REQUIRE(subject.get(key4) == &int4);
      REQUIRE(subject.get(key5) == &int5);
      REQUIRE(subject.get(key6) == &int6);
    }
    
    SUBCASE("delete internal node with 1 child (0),(5)") {
      REQUIRE(subject.del(key0) == &int0);
      REQUIRE(subject.get(key0) == nullptr);
      REQUIRE(subject.get(key1) == &int1);
      REQUIRE(subject.get(key2) == &int2);
      REQUIRE(subject.get(key3) == &int3);
      REQUIRE(subject.get(key4) == &int4);
      REQUIRE(subject.get(key5) == &int5);
      REQUIRE(subject.get(key6) == &int6);

      REQUIRE(subject.del(key5) == &int5);
      REQUIRE(subject.get(key0) == nullptr);
      REQUIRE(subject.get(key1) == &int1);
      REQUIRE(subject.get(key2) == &int2);
      REQUIRE(subject.get(key3) == &int3);
      REQUIRE(subject.get(key4) == &int4);
      REQUIRE(subject.get(key5) == nullptr);
      REQUIRE(subject.get(key6) == &int6);
    }
  }

  TEST_CASE("full lexicographic traversal") {
    int int0 = 0;
    int int1 = 1;
    int int2 = 2;
    int int3 = 4;
    int int4 = 5;
    int int5 = 5;
    int int6 = 6;

    art<int> subject;

    subject.set(key_type{'a', 'a'}, &int0);
    subject.set(key_type{'a', 'a', 'a', 'a'}, &int1);
    subject.set(key_type{'a', 'a', 'a', 'a', 'a', 'a', 'a'}, &int2);
    subject.set(key_type{'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'},
                &int3);
    subject.set(key_type{'a', 'a', 'a', 'a', 'a', 'a', 'a', 'b', 'a'}, &int4);
    subject.set(key_type{'a', 'a', 'a', 'a', 'b', 'a', 'a'}, &int5);
    subject.set(key_type{'a', 'a', 'a', 'a', 'b', 'a', 'a', 'a', 'a', 'a'},
                &int6);

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

    auto it = subject.lexicographic_it();
    auto it_end = subject.lexicographic_it_end();

    // 0
    REQUIRE(it != it_end);
    REQUIRE_EQ(&int0, *it);

    ++it;
    // 1
    REQUIRE(it != it_end);
    REQUIRE_EQ(&int1, *it);

    ++it;
    // 2
    REQUIRE(it != it_end);
    REQUIRE_EQ(&int2, *it);

    ++it;
    // 3
    REQUIRE(it != it_end);
    REQUIRE_EQ(&int3, *it);

    ++it;
    // 4
    REQUIRE(it != it_end);
    REQUIRE_EQ(&int4, *it);

    ++it;
    // 5
    REQUIRE(it != it_end);
    REQUIRE_EQ(&int5, *it);

    ++it;
    // 6
    REQUIRE(it != it_end);
    REQUIRE_EQ(&int6, *it);

    ++it;
    // 7 (overflow)
    REQUIRE(it == it_end);
  }

  TEST_CASE("range lexicographic traversal") {
    int int0 = 0;
    int int1 = 1;
    int int2 = 2;
    int int3 = 3;
    int int4 = 4;
    int int5 = 5;
    int int6 = 6;

    art<int> subject;

    subject.set(key_type{'a', 'a'}, &int0);
    subject.set(key_type{'a', 'a', 'a', 'a'}, &int1);
    subject.set(key_type{'a', 'a', 'a', 'a', 'a', 'a', 'a'}, &int2);
    subject.set(key_type{'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'},
                &int3);
    subject.set(key_type{'a', 'a', 'a', 'a', 'a', 'a', 'a', 'b', 'a'}, &int4);
    subject.set(key_type{'a', 'a', 'a', 'a', 'b', 'a', 'a'}, &int5);
    subject.set(key_type{'a', 'a', 'a', 'a', 'b', 'a', 'a', 'a', 'a', 'a'},
                &int6);

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

    // iterator on ["aaaaaaaaaa",3]
    auto it = subject.lexicographic_it(
        key_type{'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'});

    // iterator on ["aaaabaaaaa",6]
    auto it_end = subject.lexicographic_it(
        key_type{'a', 'a', 'a', 'a', 'b', 'a', 'a', 'a', 'a', 'a'});

    // 3
    REQUIRE(it != it_end);
    REQUIRE_EQ(&int3, *it);

    ++it;
    // 4
    REQUIRE(it != it_end);
    REQUIRE_EQ(&int4, *it);

    ++it;
    // 5
    REQUIRE(it != it_end);
    REQUIRE_EQ(&int5, *it);

    ++it;
    // 6
    REQUIRE(it == it_end);
  }
}
