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
    node_0<int> node;
    string key = "000100001";
    string prefix = "0000";

    node.prefix_ = (uint8_t *)prefix.c_str();
    node.prefix_len_ = prefix.length();

    CHECK_EQ(3,
             node.check_prefix((uint8_t *)key.c_str() + 0, key.length() - 0));
    CHECK_EQ(2,
             node.check_prefix((uint8_t *)key.c_str() + 1, key.length() - 1));
    CHECK_EQ(1,
             node.check_prefix((uint8_t *)key.c_str() + 2, key.length() - 2));
    CHECK_EQ(0,
             node.check_prefix((uint8_t *)key.c_str() + 3, key.length() - 3));
    CHECK_EQ(4,
             node.check_prefix((uint8_t *)key.c_str() + 4, key.length() - 4));
    CHECK_EQ(3,
             node.check_prefix((uint8_t *)key.c_str() + 5, key.length() - 5));
    CHECK_EQ(2,
             node.check_prefix((uint8_t *)key.c_str() + 6, key.length() - 6));
    CHECK_EQ(1,
             node.check_prefix((uint8_t *)key.c_str() + 7, key.length() - 7));
    CHECK_EQ(0,
             node.check_prefix((uint8_t *)key.c_str() + 8, key.length() - 8));
  }

  TEST_CASE("iteration") {
    node_4<void> m;

    node_0<void> n0;
    node_0<void> n1;
    node_0<void> n2;
    node_0<void> n3;

    m.set_child(0, &n0);
    m.set_child(5, &n1);
    m.set_child(6, &n2);
    m.set_child(255, &n3);

    auto it = m.begin();
    auto it_end = m.end();

    // 0
    REQUIRE(it < it_end);
    REQUIRE(it <= it_end);
    REQUIRE(it_end > it);
    REQUIRE(it_end >= it);
    REQUIRE(it != it_end);
    REQUIRE_EQ(0, *it);

    ++it;
    // 1
    REQUIRE(it < it_end);
    REQUIRE_EQ(5, *it);

    ++it;
    // 2
    REQUIRE(it < it_end);
    REQUIRE_EQ(6, *it);

    ++it;
    // 3
    REQUIRE(it < it_end);
    REQUIRE_EQ(255, *it);

    ++it;
    // 4 (overflow)
    REQUIRE(it == it_end);
    REQUIRE(it <= it_end);
    REQUIRE(it >= it_end);

    --it;
    // 3
    REQUIRE(it < it_end);
    REQUIRE_EQ(255, *it);

    --it;
    // 2
    REQUIRE(it < it_end);
    REQUIRE_EQ(6, *it);

    --it;
    // 1
    REQUIRE(it < it_end);
    REQUIRE_EQ(5, *it);

    --it;
    // 0
    REQUIRE(it < it_end);
    REQUIRE_EQ(0, *it);

    --it;
    // -1 (underflow)
    REQUIRE(it < it_end);

    ++it;
    // 0
    REQUIRE(it < it_end);
    REQUIRE_EQ(0, *it);
  }

  TEST_CASE("reverse iteration") {
    node_4<void> m;

    node_0<void> n0;
    node_0<void> n1;
    node_0<void> n2;
    node_0<void> n3;

    m.set_child(0, &n0);
    m.set_child(5, &n1);
    m.set_child(6, &n2);
    m.set_child(255, &n3);

    auto it = m.rbegin();
    auto it_end = m.rend();

    // 0
    REQUIRE(it < it_end);
    REQUIRE(it <= it_end);
    REQUIRE(it_end > it);
    REQUIRE(it_end >= it);
    REQUIRE(it != it_end);
    REQUIRE_EQ(255, *it);

    ++it;
    // 1
    REQUIRE_EQ(6, *it);

    ++it;
    // 2
    REQUIRE(it < it_end);
    REQUIRE(it < it_end);
    REQUIRE_EQ(5, *it);

    ++it;
    // 3
    REQUIRE(it < it_end);
    REQUIRE_EQ(0, *it);

    ++it;
    // 4 (overflow)
    REQUIRE(it == it_end);
    REQUIRE(it <= it_end);
    REQUIRE(it >= it_end);
    REQUIRE(it >= it_end);

    --it;
    // 3
    REQUIRE(it < it_end);
    REQUIRE_EQ(0, *it);

    --it;
    // 2
    REQUIRE(it < it_end);
    REQUIRE_EQ(5, *it);

    --it;
    // 1
    REQUIRE(it < it_end);
    REQUIRE_EQ(6, *it);

    --it;
    // 0
    REQUIRE(it < it_end);
    REQUIRE_EQ(255, *it);

    --it;
    // -1 (underflow)
    REQUIRE(it < it_end);

    ++it;
    // 0
    REQUIRE(it < it_end);
    REQUIRE_EQ(255, *it);
  }
}
