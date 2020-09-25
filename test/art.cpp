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

using std::array;
using std::hash;
using std::mt19937;
using std::mt19937_64;
using std::random_device;
using std::shuffle;
using std::string;
using std::to_string;

TEST_SUITE("art") {

  TEST_CASE("set") {

    art::art<int> trie;

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
      const char * prefix_key = "abc";
      const char * key = "abcde";
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
      const char *key1 = "aaaaa";
      const char *key2 = "aabaa";
      trie.set(key1, &dummy_value_1);
      trie.set(key2, &dummy_value_2);
      REQUIRE_EQ(&dummy_value_1, trie.get(key1));
      REQUIRE_EQ(&dummy_value_2, trie.get(key2));
    }

    SUBCASE("monte carlo") {
      const int n = 1000;
      string keys[n];
      int *values[n];
      /* rng */
      mt19937_64 g(0);
      for (int experiment = 0; experiment < 10; experiment += 1) {
        for (int i = 0; i < n; i += 1) {
          keys[i] = to_string(g());
          values[i] = new int();
        }

        art::art<int> m;

        for (int i = 0; i < n; i += 1) {
          m.set(keys[i].c_str(), values[i]);

          for (int j = 0; j < i; j += 1) {
            REQUIRE_EQ(values[j], m.get(keys[j].c_str()));
          }
        }

        for (int i = 0; i < n; i += 1) {
          delete values[i];
        }
      }
    }
  }

  TEST_CASE("delete value") {

    auto key0 = "aa";
    auto int0 = 0;
    auto key1 = "aaaa";
    auto int1 = 1;
    auto key2 = "aaaaaaa";
    auto int2 = 2;
    auto key3 = "aaaaaaaaaa";
    auto int3 = 3;
    auto key4 = "aaaaaaaba";
    auto int4 = 4;
    auto key5 = "aaaabaa";
    auto int5 = 5;
    auto key6 = "aaaabaaaaa";
    auto int6 = 6;
    auto key7 = "aaaaaaaaaaa";
    auto int7 = 7;
    auto key8 = "aaaaaaaaaab";
    auto int8 = 8;
    auto key9 = "aaaaaaaaaac";
    auto int9 = 9;

    art::art<int> m;

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
     *          (aa)
     *   $______/ |a  
     *   /        |     
     *  ()->0    (a)  
     *   $______/ |a\___________b 
     *   /        |             \
     *  ()->1   (aa)           (aa)
     *   $______/ |a\___b       |$\____a
     *   /        |     \       |      \
     *  ()->2 (aa$)->3 (a$)->4 ()->5 (aa) 
     *                              $/   \a
     *                              /     \
     *                             ()->6  ()->7
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
    art::art<int> m;
    mt19937_64 rng1(0);
    for (int i = 0; i < 1000000; ++i) {
      auto k = to_string(rng1());
      auto v = new int();
      *v = i;
      REQUIRE(m.set(k.c_str(), v) == nullptr);
    }
    mt19937_64 rng2(0);
    for (int i = 0; i < 1000000; ++i) {
      auto k = to_string(rng2());
      auto get_res = m.get(k.c_str());
      auto del_res = m.del(k.c_str());
      REQUIRE(m.get(k.c_str()) == nullptr);
      REQUIRE(get_res == del_res);
      REQUIRE(del_res != nullptr);
      REQUIRE(*del_res == i);
      delete del_res;
    }
  }

  TEST_CASE("full lexicographic traversal") {
    SUBCASE("controlled test") {
      int int0 = 0;
      int int1 = 1;
      int int2 = 2;
      int int3 = 4;
      int int4 = 5;
      int int5 = 5;
      int int6 = 6;

      art::art<int> m;

      m.set("aa", &int0);
      m.set("aaaa", &int1);
      m.set("aaaaaaa", &int2);
      m.set("aaaaaaaaaa", &int3);
      m.set("aaaaaaaba", &int4);
      m.set("aaaabaa", &int5);
      m.set("aaaabaaaaa", &int6);

      /* The above statements construct the following tree:
       *
       *          (aa)
       *   $_____/ |a
       *   /       |
       *  ()->0   (a)
       *   $_____/ |a\____________b
       *   /       |              \
       *  ()->1   (aa)            (aa)
       *   $_____/ |a\___b         |$\____a
       *   /       |     \         |      \
       *  ()->2 (aa$)->3 (a$)->4 ()->5 (aa$)->6
       *
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

    SUBCASE("tree_len") {
      int n = 0x10000;
      char key[5];
      int value;
      art::art<int> m;
      for (int i = 0; i < n; ++i) {
        std::snprintf(key, 5, "%04X", i);
        m.set(key, &value);
      }

      auto it = m.begin();
      auto it_end = m.end();
      int actual_n = 0;
      for (; it != it_end; ++it) {
          ++actual_n;
      }
      REQUIRE_EQ(n, actual_n);
    }
  }

  TEST_CASE("range lexicographic traversal") {
    SUBCASE("controlled test") {
      int int0 = 0;
      int int1 = 1;
      int int2 = 2;
      int int3 = 3;
      int int4 = 4;
      int int5 = 5;
      int int6 = 6;

      art::art<int> m;

      m.set("aa", &int0);
      m.set("aaaa", &int1);
      m.set("aaaaaaa", &int2);
      m.set("aaaaaaaaaa", &int3);
      m.set("aaaaaaaba", &int4);
      m.set("aaaabaa", &int5);
      m.set("aaaabaaaaa", &int6);

      /* The above statements construct the following tree:
       *
       *          (aa)
       *   $_____/ |a
       *   /       |
       *  ()->0   (a)
       *   $_____/ |a\____________b
       *   /       |              \
       *  ()->1   (aa)            (aa)
       *   $ ____/ |a\___b         |a\____$
       *    /      |     \         |      \
       *  ()->2 (aa$)->3 (a$)->4 ()->5 (aa$)->6
       *
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

    SUBCASE("monte carlo") {
      mt19937_64 rng(0);
      int n = 0x10000;
      char keys[n][4];
      int value;
      art::art<int> m;
      for (int i = 0; i < n; ++i) {
        std::snprintf(keys[i], 5, "%04X", i);
        m.set(keys[i], &value);
      }
      for (int experiment = 0; experiment < 1000; ++experiment) {
        int start = rng() % n;
        int end;
        do {
          end = rng() % n;
        } while (end < start);
        auto it = m.begin(keys[start]);
        auto it_end = m.begin(keys[end]);
        int actual_n = 0;
        for (; it != it_end; ++it) {
            ++actual_n;
        }
        REQUIRE_EQ(end - start, actual_n);
      }
    }
  }
}
