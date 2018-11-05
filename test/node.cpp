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
    leaf_node<int> node(nullptr);
    string key = "000100001";
    string prefix = "0000";

    node.prefix_ = (char *) prefix.c_str();
    node.prefix_len_ = prefix.length();

    CHECK_EQ(3, node.check_prefix(key.c_str() + 0, key.length() - 0));
    CHECK_EQ(2, node.check_prefix(key.c_str() + 1, key.length() - 1));
    CHECK_EQ(1, node.check_prefix(key.c_str() + 2, key.length() - 2));
    CHECK_EQ(0, node.check_prefix(key.c_str() + 3, key.length() - 3));
    CHECK_EQ(4, node.check_prefix(key.c_str() + 4, key.length() - 4));
    CHECK_EQ(3, node.check_prefix(key.c_str() + 5, key.length() - 5));
    CHECK_EQ(2, node.check_prefix(key.c_str() + 6, key.length() - 6));
    CHECK_EQ(1, node.check_prefix(key.c_str() + 7, key.length() - 7));
    CHECK_EQ(0, node.check_prefix(key.c_str() + 8, key.length() - 8));
  }
}
