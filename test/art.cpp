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
using std::string;
using std::to_string;
using std::mt19937_64;
using std::hash;

TEST_SUITE("art") {

  TEST_CASE("set") {

    art<int> trie;

    int dummy_value_1;
    int dummy_value_2;

    SUBCASE("insert into empty tree") {
      REQUIRE_EQ(nullptr, trie.get("abc"));
      trie.set("abc", &dummy_value_1);
      REQUIRE_EQ(&dummy_value_1, trie.get("abc"));
    }

    SUBCASE("insert into empty tree & replace") {
      trie.set("abc", &dummy_value_1);
      trie.set("abc", &dummy_value_2);
      REQUIRE_EQ(&dummy_value_2, trie.get("abc"));
    }

    SUBCASE("insert value s.t. existing value is a prefix") {
      string prefix_key = "abc";
      string key = "abcde";
      trie.set(prefix_key, &dummy_value_1);
      trie.set(key, &dummy_value_2);
      REQUIRE_EQ(&dummy_value_1, trie.get(prefix_key));
      REQUIRE_EQ(&dummy_value_2, trie.get(key));
    }

    SUBCASE("insert value s.t. new value is a prefix") {
      trie.set("abcde", &dummy_value_1);
      trie.set("abc", &dummy_value_2);
      REQUIRE_EQ(&dummy_value_1, trie.get("abcde"));
      REQUIRE_EQ(&dummy_value_2, trie.get("abc"));
    }

    SUBCASE("insert key s.t. it mismatches existing key") {
      string key1 = {'a', 'a', 'a', 'a', 'a'};
      string key2 = {'a', 'a', 'b', 'a', 'a'};
      trie.set(key1, &dummy_value_1);
      trie.set(key2, &dummy_value_2);
      REQUIRE_EQ(&dummy_value_1, trie.get(key1));
      REQUIRE_EQ(&dummy_value_2, trie.get(key2));
    }

    SUBCASE("monte carlo") {
      const int n = 1000;
      array<string, n> keys;
      array<int *, n> values;
      /* rng */
      mt19937_64 g(0);
      for (int experiment = 0; experiment < 10; experiment += 1) {
        for (int i = 0; i < n; i += 1) {
          std::string random_num_str = to_string(g());
          keys[i] = string(random_num_str.cbegin(), random_num_str.cend());
          values[i] = new int();
        }

        art<int> m;

        for (int i = 0; i < n; i += 1) {
          m.set(keys[i], values[i]);

          for (int j = 0; j < i; j += 1) {
            REQUIRE_EQ(values[j], m.get(keys[j]));
          }
        }

        for (int i = 0; i < n; i += 1) {
          delete values[i];
        }
      }
    }
  }

  TEST_CASE("delete value") {

    string key0 = "aa";
    int int0 = 0;
    string key1 = "aaaa";
    int int1 = 1;
    string key2 = "aaaaaaa";
    int int2 = 2;
    string key3 = "aaaaaaaaaa";
    int int3 = 3;
    string key4 = "aaaaaaaba";
    int int4 = 4;
    string key5 = "aaaabaa";
    int int5 = 5;
    string key6 = "aaaabaaaaa";
    int int6 = 6;
    string key7 = "aaaaaaaaaaa";
    int int7 = 7;
    string key8 = "aaaaaaaaaab";
    int int8 = 8;
    string key9 = "aaaaaaaaaac";
    int int9 = 9;

    art<int> m;

    m.set(key0, &int0);
    m.set(key1, &int1);
    m.set(key2, &int2);
    m.set(key3, &int3);
    m.set(key4, &int4);
    m.set(key5, &int5);
    m.set(key6, &int6);
    m.set(key7, &int7);
    m.set(key8, &int8);
    m.set(key9, &int9);

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
     *      a/|b\c
     *      / |  \
     *  7<-() () ()->8
     *        |
     *        9
     */

    SUBCASE("delete non existing value") {
      REQUIRE(m.del("aaaaa") == nullptr);
      REQUIRE(m.del("aaaaaa") == nullptr);
      REQUIRE(m.del("aaaab") == nullptr);
      REQUIRE(m.del("aaaaba") == nullptr);
      REQUIRE(m.get(key0) == &int0);
      REQUIRE(m.get(key1) == &int1);
      REQUIRE(m.get(key2) == &int2);
      REQUIRE(m.get(key3) == &int3);
      REQUIRE(m.get(key4) == &int4);
      REQUIRE(m.get(key5) == &int5);
      REQUIRE(m.get(key6) == &int6);
      REQUIRE(m.get(key7) == &int7);
      REQUIRE(m.get(key8) == &int8);
      REQUIRE(m.get(key9) == &int9);
    }

    SUBCASE("n_children == 0 && n_siblings == 0 (6)") {
      REQUIRE(m.del(key6) == &int6);
      REQUIRE(m.get(key0) == &int0);
      REQUIRE(m.get(key1) == &int1);
      REQUIRE(m.get(key2) == &int2);
      REQUIRE(m.get(key3) == &int3);
      REQUIRE(m.get(key4) == &int4);
      REQUIRE(m.get(key5) == &int5);
      REQUIRE(m.get(key6) == nullptr);
      REQUIRE(m.get(key7) == &int7);
      REQUIRE(m.get(key8) == &int8);
      REQUIRE(m.get(key9) == &int9);
    }

    SUBCASE("n_children == 0 && n_siblings == 1 (4)") {
      REQUIRE(m.del(key4) == &int4);
      REQUIRE(m.get(key0) == &int0);
      REQUIRE(m.get(key1) == &int1);
      REQUIRE(m.get(key2) == &int2);
      REQUIRE(m.get(key3) == &int3);
      REQUIRE(m.get(key4) == nullptr);
      REQUIRE(m.get(key5) == &int5);
      REQUIRE(m.get(key6) == &int6);
      REQUIRE(m.get(key7) == &int7);
      REQUIRE(m.get(key8) == &int8);
      REQUIRE(m.get(key9) == &int9);
    }

    SUBCASE("n_children == 0 && n_siblings > 1 (7)") {
      REQUIRE(m.del(key7) == &int7);
      REQUIRE(m.get(key0) == &int0);
      REQUIRE(m.get(key1) == &int1);
      REQUIRE(m.get(key2) == &int2);
      REQUIRE(m.get(key3) == &int3);
      REQUIRE(m.get(key4) == &int4);
      REQUIRE(m.get(key5) == &int5);
      REQUIRE(m.get(key6) == &int6);
      REQUIRE(m.get(key7) == nullptr);
      REQUIRE(m.get(key8) == &int8);
      REQUIRE(m.get(key9) == &int9);
    }

    SUBCASE("n_children == 1 (0),(5)") {
      REQUIRE(m.del(key0) == &int0);
      REQUIRE(m.get(key0) == nullptr);
      REQUIRE(m.get(key1) == &int1);
      REQUIRE(m.get(key2) == &int2);
      REQUIRE(m.get(key3) == &int3);
      REQUIRE(m.get(key4) == &int4);
      REQUIRE(m.get(key5) == &int5);
      REQUIRE(m.get(key6) == &int6);
      REQUIRE(m.get(key7) == &int7);
      REQUIRE(m.get(key8) == &int8);
      REQUIRE(m.get(key9) == &int9);

      REQUIRE(m.del(key5) == &int5);
      REQUIRE(m.get(key0) == nullptr);
      REQUIRE(m.get(key1) == &int1);
      REQUIRE(m.get(key2) == &int2);
      REQUIRE(m.get(key3) == &int3);
      REQUIRE(m.get(key4) == &int4);
      REQUIRE(m.get(key5) == nullptr);
      REQUIRE(m.get(key6) == &int6);
      REQUIRE(m.get(key7) == &int7);
      REQUIRE(m.get(key8) == &int8);
      REQUIRE(m.get(key9) == &int9);
    }

    SUBCASE("n_children > 1 (3),(2),(1)") {
      REQUIRE(m.del(key3) == &int3);
      REQUIRE(m.get(key0) == &int0);
      REQUIRE(m.get(key1) == &int1);
      REQUIRE(m.get(key2) == &int2);
      REQUIRE(m.get(key3) == nullptr);
      REQUIRE(m.get(key4) == &int4);
      REQUIRE(m.get(key5) == &int5);
      REQUIRE(m.get(key6) == &int6);
      REQUIRE(m.get(key7) == &int7);
      REQUIRE(m.get(key8) == &int8);
      REQUIRE(m.get(key9) == &int9);

      REQUIRE(m.del(key2) == &int2);
      REQUIRE(m.get(key0) == &int0);
      REQUIRE(m.get(key1) == &int1);
      REQUIRE(m.get(key2) == nullptr);
      REQUIRE(m.get(key3) == nullptr);
      REQUIRE(m.get(key4) == &int4);
      REQUIRE(m.get(key5) == &int5);
      REQUIRE(m.get(key6) == &int6);
      REQUIRE(m.get(key7) == &int7);
      REQUIRE(m.get(key8) == &int8);
      REQUIRE(m.get(key9) == &int9);

      REQUIRE(m.del(key1) == &int1);
      REQUIRE(m.get(key0) == &int0);
      REQUIRE(m.get(key1) == nullptr);
      REQUIRE(m.get(key2) == nullptr);
      REQUIRE(m.get(key3) == nullptr);
      REQUIRE(m.get(key4) == &int4);
      REQUIRE(m.get(key5) == &int5);
      REQUIRE(m.get(key6) == &int6);
      REQUIRE(m.get(key7) == &int7);
      REQUIRE(m.get(key8) == &int8);
      REQUIRE(m.get(key9) == &int9);
    }
  }

  TEST_CASE("monte carlo delete") {
    hash<string> h;
    art<int> m;
    mt19937_64 rng1(0);
    for(int i = 0; i < 1000000; ++i) {
      auto k = to_string(rng1());
      int *v = new int();
      *v = i;
      REQUIRE(m.set(k, v) == nullptr);
    }
    mt19937_64 rng2(0);
    for(int i = 0; i < 1000000; ++i) {
      auto k = to_string(rng2());
      auto get_res = m.get(k);
      auto del_res = m.del(k);
      REQUIRE(m.get(k) == nullptr);
      REQUIRE(get_res == del_res);
      REQUIRE(del_res != nullptr);
      REQUIRE(*del_res == i);
      delete del_res;
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

    art<int> m;

    m.set("aa", &int0);
    m.set("aaaa", &int1);
    m.set("aaaaaaa", &int2);
    m.set("aaaaaaaaaa", &int3);
    m.set("aaaaaaaba", &int4);
    m.set("aaaabaa", &int5);
    m.set("aaaabaaaaa", &int6);

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

    auto it = m.begin();
    auto it_end = m.end();

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

    art<int> m;

    m.set("aa", &int0);
    m.set("aaaa", &int1);
    m.set("aaaaaaa", &int2);
    m.set("aaaaaaaaaa", &int3);
    m.set("aaaaaaaba", &int4);
    m.set("aaaabaa", &int5);
    m.set("aaaabaaaaa", &int6);

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
    auto it = m.begin("aaaaaaaaaa");

    // iterator on ["aaaabaaaaa",6]
    auto it_end = m.begin("aaaabaaaaa");

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
