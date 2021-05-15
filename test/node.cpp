/**
 * @file node abstract class tests
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "doctest.h"
#include <algorithm>
#include <array>
#include <iostream>
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
using std::string;

TEST_SUITE("node") {

  TEST_CASE("check_prefix") {
    string key = "000100001";
    int key_len = key.length() + 1; // +1 for \0

    string prefix = "0000";
    int prefix_len = prefix.length() + 1; // +1 for \0
    leaf_node<int*> node(prefix.c_str(), prefix_len, nullptr);

    CHECK_EQ(3, node.check_prefix(key.c_str() + 0, key_len - 0));
    CHECK_EQ(2, node.check_prefix(key.c_str() + 1, key_len - 1));
    CHECK_EQ(1, node.check_prefix(key.c_str() + 2, key_len - 2));
    CHECK_EQ(0, node.check_prefix(key.c_str() + 3, key_len - 3));
    CHECK_EQ(4, node.check_prefix(key.c_str() + 4, key_len - 4));
    CHECK_EQ(3, node.check_prefix(key.c_str() + 5, key_len - 5));
    CHECK_EQ(2, node.check_prefix(key.c_str() + 6, key_len - 6));
    CHECK_EQ(1, node.check_prefix(key.c_str() + 7, key_len - 7));
    CHECK_EQ(0, node.check_prefix(key.c_str() + 8, key_len - 8));
    CHECK_EQ(0, node.check_prefix(key.c_str() + 9, key_len - 9));
  }
}
