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

TEST_SUITE("node 256") {

  TEST_CASE("monte carlo") {
    /* set up */
    array<char, 256> partial_keys;
    array<node_0<void>, 256> children;

    for (int i = 0; i < 256; i += 1) {
      /* populate partial_keys with all values in the char domain */
      partial_keys[i] = i - 128;
    }

    /* rng */
    mt19937 g(0);

    for (int experiment = 0; experiment < 100; experiment += 1) {
      /* test subject */
      node_256<void> node;

      /* shuffle in order to make a seemingly random insertion order */
      shuffle(partial_keys.begin(), partial_keys.end(), g);
      for (int i = 0; i < 256; i += 1) {
        REQUIRE_FALSE(node.is_full());

        auto partial_key = partial_keys[i];
        auto child = &children[i];
        node.set_child(partial_key, child);

        for (int j = 0; j <= i; j += 1) {
          auto p_k = partial_keys[j];
          auto expected_child = &children[j];
          auto actual_child_ptr = node.find_child(p_k);
          REQUIRE(actual_child_ptr != nullptr);
          auto actual_child = *actual_child_ptr;
          REQUIRE_EQ(expected_child, actual_child);
        }
      }
      REQUIRE(node.is_full());
    }
  }
  
  TEST_CASE("delete child") {
    node_0<void> n0;
    node_0<void> n1;
    node_0<void> n2;
    node_0<void> n3;
    node_0<void> n4;
    node_0<void> n5;
    node_0<void> n6;

    node_256<void> subject;

    subject.set_child(1, &n1);
    subject.set_child(2, &n2);
    subject.set_child(4, &n4);
    subject.set_child(5, &n5);

    SUBCASE("delete child that doesn't exist (0)") {
      REQUIRE(subject.del_child(0) == nullptr);
      REQUIRE(*subject.find_child(1) == &n1);
      REQUIRE(*subject.find_child(2) == &n2);
      REQUIRE(*subject.find_child(4) == &n4);
      REQUIRE(*subject.find_child(5) == &n5);
    }
    
    SUBCASE("delete min (1)") {
      REQUIRE(subject.del_child(1) == &n1);
      REQUIRE(subject.find_child(1) == nullptr);
      REQUIRE(*subject.find_child(2) == &n2);
      REQUIRE(*subject.find_child(4) == &n4);
      REQUIRE(*subject.find_child(5) == &n5);
    }

    SUBCASE("delete inner (2)") {
      REQUIRE(subject.del_child(2) == &n2);
      REQUIRE(*subject.find_child(1) == &n1);
      REQUIRE(subject.find_child(2) == nullptr);
      REQUIRE(*subject.find_child(4) == &n4);
      REQUIRE(*subject.find_child(5) == &n5);
    }
    
    SUBCASE("delete child that doesn't exist (3)") {
      REQUIRE(subject.del_child(3) == nullptr);
      REQUIRE(*subject.find_child(1) == &n1);
      REQUIRE(*subject.find_child(2) == &n2);
      REQUIRE(*subject.find_child(4) == &n4);
      REQUIRE(*subject.find_child(5) == &n5);
    }

    SUBCASE("delete inner (4)") {
      REQUIRE(subject.del_child(4) == &n4);
      REQUIRE(*subject.find_child(1) == &n1);
      REQUIRE(*subject.find_child(2) == &n2);
      REQUIRE(subject.find_child(4) == nullptr);
      REQUIRE(*subject.find_child(5) == &n5);
    }

    SUBCASE("delete max (5)") {
      REQUIRE(subject.del_child(5) == &n5);
      REQUIRE(*subject.find_child(1) == &n1);
      REQUIRE(*subject.find_child(2) == &n2);
      REQUIRE(*subject.find_child(4) == &n4);
      REQUIRE(subject.find_child(5) == nullptr);
    }
    
    SUBCASE("delete child that doesn't exist (6)") {
      REQUIRE(subject.del_child(6) == nullptr);
      REQUIRE(*subject.find_child(1) == &n1);
      REQUIRE(*subject.find_child(2) == &n2);
      REQUIRE(*subject.find_child(4) == &n4);
      REQUIRE(*subject.find_child(5) == &n5);
    }
  }

  TEST_CASE("next partial key") {
    node_256<void> n;

    SUBCASE("completely empty node") {
      REQUIRE_THROWS_AS(n.next_partial_key(-128), std::out_of_range);
    }

    SUBCASE("child at -128") {
      node_0<void> n0;
      n.set_child(-128, &n0);
      REQUIRE_EQ(-128, n.next_partial_key(-128));
      for (int i = 1; i < 256; ++i) {
        REQUIRE_THROWS_AS(n.next_partial_key(i - 128), std::out_of_range);
      }
    }

    SUBCASE("child at 127") {
      node_0<void> n0;
      n.set_child(127, &n0);
      for (int i = 0; i < 256; ++i) {
        REQUIRE_EQ(127, n.next_partial_key(i - 128));
      }
    }

    SUBCASE("dense children") {
      node_0<void> n0;
      node_0<void> n1;
      node_0<void> n2;
      node_0<void> n3;
      n.set_child(0, &n0);
      n.set_child(1, &n1);
      n.set_child(2, &n2);
      n.set_child(3, &n3);
      REQUIRE_EQ(0, n.next_partial_key(0));
      REQUIRE_EQ(1, n.next_partial_key(1));
      REQUIRE_EQ(2, n.next_partial_key(2));
      REQUIRE_EQ(3, n.next_partial_key(3));
      REQUIRE_THROWS_AS(n.next_partial_key(4), std::out_of_range);
    }

    SUBCASE("sparse children") {
      node_0<void> n0;
      node_0<void> n1;
      node_0<void> n2;
      node_0<void> n3;
      n.set_child(0, &n0);
      n.set_child(5, &n1);
      n.set_child(10, &n2);
      n.set_child(100, &n3);
      REQUIRE_EQ(0, n.next_partial_key(0));
      REQUIRE_EQ(5, n.next_partial_key(1));
      REQUIRE_EQ(10, n.next_partial_key(6));
      REQUIRE_EQ(100, n.next_partial_key(11));
      REQUIRE_THROWS_AS(n.next_partial_key(101), std::out_of_range);
    }
  }
  
  TEST_CASE("previous partial key") {
    node_256<void> n;

    SUBCASE("completely empty node") {
      for (int i = 0; i < 256; ++i) {
        REQUIRE_THROWS_AS(n.prev_partial_key(i - 128), std::out_of_range);
      }
    }

    SUBCASE("child at -128") {
      node_0<void> n0;
      n.set_child(-128, &n0);
      for (int i = 0; i < 256; ++i) {
        REQUIRE_EQ(-128, n.prev_partial_key(i - 128));
      }
    }

    SUBCASE("child at 127") {
      node_0<void> n0;
      n.set_child(127, &n0);
      REQUIRE_EQ(127, n.prev_partial_key(127));
      for (int i = 0; i < 255; ++i) {
        REQUIRE_THROWS_AS(n.prev_partial_key(i - 128), std::out_of_range);
      }
    }

    SUBCASE("dense children") {
      node_0<void> n0;
      node_0<void> n1;
      node_0<void> n2;
      node_0<void> n3;
      n.set_child(1, &n0);
      n.set_child(2, &n1);
      n.set_child(3, &n2);
      n.set_child(4, &n3);
      REQUIRE_EQ(1, n.prev_partial_key(1));
      REQUIRE_EQ(2, n.prev_partial_key(2));
      REQUIRE_EQ(3, n.prev_partial_key(3));
      REQUIRE_EQ(4, n.prev_partial_key(4));
      REQUIRE_EQ(4, n.prev_partial_key(127));
      REQUIRE_THROWS_AS(n.prev_partial_key(0), std::out_of_range);
    }
    
    SUBCASE("sparse children") {
      node_0<void> n0;
      node_0<void> n1;
      node_0<void> n2;
      node_0<void> n3;
      n.set_child(1, &n0);
      n.set_child(5, &n1);
      n.set_child(10, &n2);
      n.set_child(100, &n3);
      REQUIRE_EQ(1, n.prev_partial_key(4));
      REQUIRE_EQ(5, n.prev_partial_key(9));
      REQUIRE_EQ(10, n.prev_partial_key(99));
      REQUIRE_EQ(100, n.prev_partial_key(127));
      REQUIRE_THROWS_AS(n.prev_partial_key(0), std::out_of_range);
    }
  }
}
