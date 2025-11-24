/**
 * @file node_16 tests
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

TEST_SUITE("node 16") {

  TEST_CASE("monte carlo") {
    /* set up */
    array<uint8_t, 256> partial_keys;
    array<node<void*> *, 256> children;

    for (int i = 0; i < 256; i += 1) {
      /* populate partial_keys with all values in the partial_keys_t domain */
      partial_keys[i] = i;

      /* populate child nodes */
      children[i] = new leaf_node<void*>(nullptr);
    }

    /* rng */
    random_device rd;
    mt19937 g(rd());

    for (int experiment = 0; experiment < 1000; experiment += 1) {
      /* test subject */
      node_16<void*> node;

      /* shuffle in order to make a seemingly random insertion order */
      shuffle(partial_keys.begin(), partial_keys.end(), g);
      for (int i = 0; i < 16; i += 1) {
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
    leaf_node<void*> n0(nullptr);
    leaf_node<void*> n1(nullptr);
    leaf_node<void*> n2(nullptr);
    leaf_node<void*> n3(nullptr);
    leaf_node<void*> n4(nullptr);
    leaf_node<void*> n5(nullptr);
    leaf_node<void*> n6(nullptr);

    node_16<void*> subject;

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
    node_16<void*> n;

    SUBCASE("completely empty node") {
      REQUIRE_THROWS_AS(n.next_partial_key(0), std::out_of_range);
    }

    SUBCASE("child at -128") {
      n.set_child(-128, nullptr);
      REQUIRE_EQ(-128, n.next_partial_key(-128));
      for (int i = 1; i < 256; ++i) {
        REQUIRE_THROWS_AS(n.next_partial_key(i - 128), std::out_of_range);
      }
    }


    SUBCASE("child at 127") {
      n.set_child(127, nullptr);
      for (int i = 0; i < 256; ++i) {
        REQUIRE_EQ(127, n.next_partial_key(i - 128));
      }
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
    node_16<void*> n;

    SUBCASE("completely empty node") {
      REQUIRE_THROWS_AS(n.prev_partial_key(127), std::out_of_range);
    }

    SUBCASE("child at -128") {
      n.set_child(-128, nullptr);
      for (int i = 0; i < 256; ++i) {
        REQUIRE_EQ(-128, n.prev_partial_key(i - 128));
      }
    }


    SUBCASE("child at 127") {
      n.set_child(127, nullptr);
      REQUIRE_EQ(127, n.prev_partial_key(127));
      for (int i = 0; i < 255; ++i) {
        REQUIRE_THROWS_AS(n.prev_partial_key(i - 128), std::out_of_range);
      }
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
      REQUIRE_EQ(4, n.prev_partial_key(127));
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
      REQUIRE_EQ(100, n.prev_partial_key(127));
      REQUIRE_THROWS_AS(n.prev_partial_key(0), std::out_of_range);
    }
  }

  TEST_CASE("grow to node_48 preserves offset (PR #20)") {
    // This test reproduces the bug reported in PR #20:
    // When node_16 grows to node_48, the indexes_ array must use 128 offset

    SUBCASE("next_partial_key after grow") {
      leaf_node<void*>* dummy_children[17];
      for (int i = 0; i < 17; ++i) {
        dummy_children[i] = new leaf_node<void*>(nullptr);
      }
      node_16<void*>* n16 = new node_16<void*>();
      char test_keys[17];
      for (int i = 0; i < 17; ++i) {
        test_keys[i] = 'a' + i; // a, b, c, ..., q
      }
      for (int i = 0; i < 16; ++i) {
        n16->set_child(test_keys[i], dummy_children[i]);
      }
      REQUIRE(n16->is_full());
      auto* n48 = static_cast<node_48<void*>*>(n16->grow());
      REQUIRE(n48 != nullptr);
      for (int i = 0; i < 16; ++i) {
        auto** child_ptr = n48->find_child(test_keys[i]);
        REQUIRE(child_ptr != nullptr);
        REQUIRE(*child_ptr == dummy_children[i]);
      }
      n48->set_child(test_keys[16], dummy_children[16]);
      for (int i = 0; i < 17; ++i) {
        auto** child_ptr = n48->find_child(test_keys[i]);
        REQUIRE(child_ptr != nullptr);
        REQUIRE(*child_ptr == dummy_children[i]);
      }
      // Starting from first key 'a', we should find all keys in order
      char current = n48->next_partial_key('a');
      REQUIRE_EQ('a', current);
      for (int i = 1; i < 17; ++i) {
        current = n48->next_partial_key(current + 1);
        REQUIRE_EQ(test_keys[i], current);
      }
      delete n48;
      for (int i = 0; i < 17; ++i) {
        delete dummy_children[i];
      }
    }

    SUBCASE("prev_partial_key after grow") {
      leaf_node<void*>* dummy_children[17];
      for (int i = 0; i < 17; ++i) {
        dummy_children[i] = new leaf_node<void*>(nullptr);
      }
      node_16<void*>* n16 = new node_16<void*>();
      char test_keys[17];
      for (int i = 0; i < 17; ++i) {
        test_keys[i] = 'a' + i; // a, b, c, ..., q
      }
      for (int i = 0; i < 16; ++i) {
        n16->set_child(test_keys[i], dummy_children[i]);
      }
      REQUIRE(n16->is_full());
      auto* n48 = static_cast<node_48<void*>*>(n16->grow());
      REQUIRE(n48 != nullptr);
      for (int i = 0; i < 16; ++i) {
        auto** child_ptr = n48->find_child(test_keys[i]);
        REQUIRE(child_ptr != nullptr);
        REQUIRE(*child_ptr == dummy_children[i]);
      }
      n48->set_child(test_keys[16], dummy_children[16]);
      for (int i = 0; i < 17; ++i) {
        auto** child_ptr = n48->find_child(test_keys[i]);
        REQUIRE(child_ptr != nullptr);
        REQUIRE(*child_ptr == dummy_children[i]);
      }
      // Starting from last key 'q', we should find all keys in reverse order
      char current = n48->prev_partial_key('q');
      REQUIRE_EQ('q', current);
      for (int i = 15; i >= 0; --i) {
        current = n48->prev_partial_key(current - 1);
        REQUIRE_EQ(test_keys[i], current);
      }
      delete n48;
      for (int i = 0; i < 17; ++i) {
        delete dummy_children[i];
      }
    }
  }
}
