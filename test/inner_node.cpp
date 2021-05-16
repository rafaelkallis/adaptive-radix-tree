/**
 * @file inner node abstract class tests
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

TEST_SUITE("inner_node") {

  TEST_CASE("iteration") {
    node_4<void*> m;

    leaf_node<void*> n0(nullptr, 0, nullptr);
    leaf_node<void*> n1(nullptr, 0, nullptr);
    leaf_node<void*> n2(nullptr, 0, nullptr);
    leaf_node<void*> n3(nullptr, 0, nullptr);

    m.set_child(0, &n0);
    m.set_child(5, &n1);
    m.set_child(6, &n2);
    m.set_child(127, &n3);

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
    REQUIRE_EQ(127, *it);

    ++it;
    // 4 (overflow)
    REQUIRE(it == it_end);
    REQUIRE(it <= it_end);
    REQUIRE(it >= it_end);

    --it;
    // 3
    REQUIRE(it < it_end);
    REQUIRE_EQ(127, *it);

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
    node_4<void*> m;

    leaf_node<void*> n0(nullptr, 0, nullptr);
    leaf_node<void*> n1(nullptr, 0, nullptr);
    leaf_node<void*> n2(nullptr, 0, nullptr);
    leaf_node<void*> n3(nullptr, 0, nullptr);

    m.set_child(0, &n0);
    m.set_child(5, &n1);
    m.set_child(6, &n2);
    m.set_child(127, &n3);

    auto it = m.rbegin();
    auto it_end = m.rend();

    // 0
    REQUIRE(it < it_end);
    REQUIRE(it <= it_end);
    REQUIRE(it_end > it);
    REQUIRE(it_end >= it);
    REQUIRE(it != it_end);
    REQUIRE_EQ(127, (int) *it);

    ++it;
    // 1
    REQUIRE_EQ(6, (int) *it);

    ++it;
    // 2
    REQUIRE(it != it_end);
    REQUIRE_EQ(5, (int) *it);

    ++it;
    // 3
    REQUIRE(it != it_end);
    REQUIRE_EQ(0, (int) *it);

    ++it;
    // 4 (overflow)
    REQUIRE(it == it_end);
    REQUIRE(it <= it_end);
    REQUIRE(it >= it_end);

    --it;
    // 3
    REQUIRE(it != it_end);
    REQUIRE_EQ(0, (int) *it);

    --it;
    // 2
    REQUIRE(it != it_end);
    REQUIRE_EQ(5, (int) *it);

    --it;
    // 1
    REQUIRE(it != it_end);
    REQUIRE_EQ(6, (int) *it);

    --it;
    // 0
    REQUIRE(it != it_end);
    REQUIRE_EQ(127, (int) *it);

    --it;
    // -1 (underflow)
    REQUIRE(it != it_end);

    ++it;
    // 0
    REQUIRE(it != it_end);
    REQUIRE_EQ(127, (int) *it);
  }
}
