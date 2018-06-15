/**
 * @file node abstract class tests
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "doctest.h"
#include <algorithm>
#include <array>
#include <memory>
#include <random>
#include <utility>
#include <vector>

using namespace ::art;

using std::array;
using std::make_pair;
using std::make_shared;
using std::mt19937;
using std::pair;
using std::random_device;
using std::shared_ptr;
using std::shuffle;

template <class T> class node_concrete : public node<T> {
public:
  node<T> **find_child(const partial_key_type & /* partial_key */) override {
    return nullptr;
  }
  void set_child(const partial_key_type & /* partial_key */,
                 node<T> * /* child */) override {}
  node<T> *grow() override { return nullptr; }
  bool is_full() const override { return true; }
  bool is_leaf() const override { return true; }

  typename node<T>::iterator begin() override {
    return node_concrete<T>::iterator();
  };
  typename node<T>::iterator end() override {
    return node_concrete<T>::iterator();
  };

  class iterator : public node<T>::iterator {};
};

TEST_CASE("node") {

  node_concrete<int> node;

  SUBCASE("check_prefix") {
    key_type key = {0, 0, 0, 1, 0, 0, 0, 0, 1};

    node.set_prefix(key_type{0, 0, 0, 0});

    CHECK_EQ(3, node.check_prefix(key, 0));
    CHECK_EQ(2, node.check_prefix(key, 1));
    CHECK_EQ(1, node.check_prefix(key, 2));
    CHECK_EQ(0, node.check_prefix(key, 3));
    CHECK_EQ(4, node.check_prefix(key, 4));
    CHECK_EQ(3, node.check_prefix(key, 5));
    CHECK_EQ(2, node.check_prefix(key, 6));
    CHECK_EQ(1, node.check_prefix(key, 7));
    CHECK_EQ(0, node.check_prefix(key, 8));
  }
}
