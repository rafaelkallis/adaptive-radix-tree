/**
 * @file node_16 benchmarks
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "picobench/picobench.hpp"

using namespace art;
using picobench::state;

PICOBENCH_SUITE("node_16");

static void node_16_constructor(state &s) {
  for (auto i __attribute__((unused)) : s) {
    node_16<int> n;
  }
}
PICOBENCH(node_16_constructor);

static void node_16_find_child(state &s) {
  node_16<int> n;
  leaf_node<int> child(nullptr);
  for (int i = 0; i < 16; ++i) {
    n.set_child((i * 17) - 128, &child);
  }
  for (auto i __attribute__((unused)) : s) {
    auto child_out __attribute__((unused)) = n.find_child(((rand() % 16) * 17) - 128);
  }
}
PICOBENCH(node_16_find_child);

static void node_16_set_child(state &s) {
  auto n = new node_16<int>();
  leaf_node<int> child(nullptr);
  for (auto i __attribute__((unused)) : s) {
    if (n->is_full()) {
      delete n;
      n = new node_16<int>();
    }
    n->set_child(((rand() % 16) * 17) - 128, &child);
  }
}
PICOBENCH(node_16_set_child);

static void node_16_grow(state &s) {
  for (auto i __attribute__((unused)) : s) {
    auto *n = new node_16<int>();
    auto *new_n = n->grow();
    delete new_n;
  }
}
PICOBENCH(node_16_grow);

static void node_16_next_partial_key(state &s) {
  node_16<int> n;
  leaf_node<int> child(nullptr);
  for (int i = 0; i < 16; ++i) {
    n.set_child((i * 17) - 128, &child);
  }
  for (auto i __attribute__((unused)) : s) {
    auto next_partial_key __attribute__((unused)) = n.next_partial_key((rand() % 256) - 128);
  }
}
PICOBENCH(node_16_next_partial_key);

static void node_16_prev_partial_key(state &s) {
  node_16<int> n;
  leaf_node<int> child(nullptr);
  for (int i = 0; i < 16; ++i) {
    n.set_child((i * 17) - 128, &child);
  }
  for (auto i __attribute__((unused)) : s) {
    auto prev_partial_key __attribute__((unused)) = n.prev_partial_key((rand() % 256) - 128);
  }
}
PICOBENCH(node_16_prev_partial_key);
