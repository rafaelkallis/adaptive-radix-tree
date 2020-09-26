/**
 * @file node_4 benchmarks
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "picobench/picobench.hpp"

using namespace art;
using picobench::state;

PICOBENCH_SUITE("node_4");

static void node_4_constructor(state &s) {
  for (auto i __attribute__((unused)) : s) {
    node_4<int> n;
  }
}
PICOBENCH(node_4_constructor);

static void node_4_find_child(state &s) {
  node_4<int> n;

  leaf_node<int> c0(nullptr);
  leaf_node<int> c1(nullptr);
  leaf_node<int> c2(nullptr);
  leaf_node<int> c3(nullptr);

  n.set_child(-128, &c0);
  n.set_child(-43, &c1);
  n.set_child(42, &c2);
  n.set_child(127, &c3);

  char partial_keys[] = {-128, -43, 42, 127};
  node<int> **child = nullptr;
  for (auto i __attribute__((unused)) : s) {
    child = n.find_child(partial_keys[rand() % 4]);
  }
}
PICOBENCH(node_4_find_child);

static void node_4_set_child(state &s) {
  auto n = new node_4<int>();
  leaf_node<int> child(nullptr);
  char partial_keys[] = {-128, -43, 42, 127};
  for (auto i __attribute__((unused)) : s) {
    if (n->is_full()) {
      delete n;
      n = new node_4<int>();
    }

    n->set_child(partial_keys[rand() % 4], &child);
  }
}
PICOBENCH(node_4_set_child);

static void node_4_grow(state &s) {
  inner_node<int> *n = nullptr;
  inner_node<int> *new_n = nullptr;
  for (auto i __attribute__((unused)) : s) {
    n = new node_4<int>();
    new_n = n->grow();
    delete new_n;
  }
}
PICOBENCH(node_4_grow);

static void node_4_next_partial_key(state &s) {
  node_4<int> n;

  leaf_node<int> c0(nullptr);
  leaf_node<int> c1(nullptr);
  leaf_node<int> c2(nullptr);
  leaf_node<int> c3(nullptr);

  n.set_child(-128, &c0);
  n.set_child(-43, &c1);
  n.set_child(42, &c2);
  n.set_child(127, &c3);

  for (auto i __attribute__((unused)) : s) {
    auto next_partial_key = n.next_partial_key((rand() % 256) - 128);
  }
}
PICOBENCH(node_4_next_partial_key);

static void node_4_prev_partial_key(state &s) {
  node_4<int> n;

  leaf_node<int> c0(nullptr);
  leaf_node<int> c1(nullptr);
  leaf_node<int> c2(nullptr);
  leaf_node<int> c3(nullptr);

  n.set_child(-128, &c0);
  n.set_child(-43, &c1);
  n.set_child(42, &c2);
  n.set_child(127, &c3);

  for (auto i __attribute__((unused)) : s) {
    auto prev_partial_key = n.prev_partial_key((rand() % 256) - 128);
  }
}
PICOBENCH(node_4_prev_partial_key);
