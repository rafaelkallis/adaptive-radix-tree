/**
 * @file node_4 tests
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "doctest.h"
#include <algorithm>
#include <array>
#include <random>
#include <stdexcept>
#include <vector>

using namespace art;

using std::array;
using std::mt19937;
using std::random_device;
using std::shuffle;

TEST_SUITE("node 4") {

  TEST_CASE("monte carlo insert") {
    /* set up */
    array<uint8_t, 256> partial_keys;
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

    for (int experiment = 0; experiment < 10000; experiment += 1) {
      /* test subject */
      node_4<void> node;

      /* shuffle in order to make a seemingly random insertion order */
      shuffle(partial_keys.begin(), partial_keys.end(), g);
      for (int i = 0; i < 4; i += 1) {
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

  TEST_CASE("delete child") {
    node_0<void> n0;
    node_0<void> n1;
    node_0<void> n2;
    node_0<void> n3;
    node_0<void> n4;
    node_0<void> n5;
    node_0<void> n6;

    node_4<void> subject;

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
    node_4<void> n;

    SUBCASE("completely empty node") {
      REQUIRE_THROWS_AS(n.next_partial_key(0), std::out_of_range);
    }

    SUBCASE("child at 0") {
      n.set_child(0, nullptr);
      REQUIRE_EQ(0, n.next_partial_key(0));
      REQUIRE_THROWS_AS(n.next_partial_key(1), std::out_of_range);
    }

    SUBCASE("child at 255") {
      n.set_child(255, nullptr);
      REQUIRE_EQ(255, n.next_partial_key(0));
      REQUIRE_EQ(255, n.next_partial_key(255));
    }

    SUBCASE("dense children") {
      n.set_child(0, nullptr);
      n.set_child(1, nullptr);
      n.set_child(2, nullptr);
      n.set_child(3, nullptr);
      REQUIRE_EQ(0, n.next_partial_key(0));
      REQUIRE_EQ(1, n.next_partial_key(1));
      REQUIRE_EQ(2, n.next_partial_key(2));
      REQUIRE_EQ(3, n.next_partial_key(3));
      REQUIRE_THROWS_AS(n.next_partial_key(4), std::out_of_range);
    }

    SUBCASE("sparse children") {
      n.set_child(0, nullptr);
      n.set_child(5, nullptr);
      n.set_child(10, nullptr);
      n.set_child(100, nullptr);
      REQUIRE_EQ(0, n.next_partial_key(0));
      REQUIRE_EQ(5, n.next_partial_key(1));
      REQUIRE_EQ(10, n.next_partial_key(6));
      REQUIRE_EQ(100, n.next_partial_key(11));
      REQUIRE_THROWS_AS(n.next_partial_key(101), std::out_of_range);
    }
  }

  TEST_CASE("previous partial key") {
    node_4<void> n;

    SUBCASE("completely empty node") {
      REQUIRE_THROWS_AS(n.prev_partial_key(255), std::out_of_range);
    }

    SUBCASE("child at 0") {
      n.set_child(0, nullptr);
      REQUIRE_EQ(0, n.prev_partial_key(0));
      REQUIRE_EQ(0, n.prev_partial_key(255));
    }

    SUBCASE("child at 255") {
      n.set_child(255, nullptr);
      REQUIRE_EQ(255, n.prev_partial_key(255));
      REQUIRE_THROWS_AS(n.prev_partial_key(254), std::out_of_range);
    }

    SUBCASE("dense children") {
      n.set_child(1, nullptr);
      n.set_child(2, nullptr);
      n.set_child(3, nullptr);
      n.set_child(4, nullptr);
      REQUIRE_EQ(1, n.prev_partial_key(1));
      REQUIRE_EQ(2, n.prev_partial_key(2));
      REQUIRE_EQ(3, n.prev_partial_key(3));
      REQUIRE_EQ(4, n.prev_partial_key(4));
      REQUIRE_EQ(4, n.prev_partial_key(255));
      REQUIRE_THROWS_AS(n.prev_partial_key(0), std::out_of_range);
    }

    SUBCASE("sparse children") {
      n.set_child(1, nullptr);
      n.set_child(5, nullptr);
      n.set_child(10, nullptr);
      n.set_child(100, nullptr);
      REQUIRE_EQ(1, n.prev_partial_key(4));
      REQUIRE_EQ(5, n.prev_partial_key(9));
      REQUIRE_EQ(10, n.prev_partial_key(99));
      REQUIRE_EQ(100, n.prev_partial_key(255));
      REQUIRE_THROWS_AS(n.prev_partial_key(0), std::out_of_range);
    }
  }
}
