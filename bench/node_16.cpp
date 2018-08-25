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
  for (auto _ : s) {
    node_16<void *> n;
  }
}
PICOBENCH(node_16_constructor);

static void node_16_find_child(state &s) {
  node_16<void *> n;
  node_0<void *> child;
  for (int i = 0; i < 16; ++i) {
    n.set_child((i * 17) - 128, &child);
  }
  for (auto _ : s) {
    auto child = n.find_child(((rand() % 16) * 17) - 128);
  }
}
PICOBENCH(node_16_find_child);

static void node_16_set_child(state &s) {
  node_16<void *> *n = new node_16<void *>();
  node_0<void *> child;
  for (auto _ : s) {
    if (n->is_full()) {
      delete n;
      n = new node_16<void *>();
    }
    n->set_child(((rand() % 16) * 17) - 128, &child);
  }
}
PICOBENCH(node_16_set_child);

static void node_16_grow(state &s) {
  for (auto _ : s) {
    node_16<void *> *n = new node_16<void *>();
    node<void *> *new_n = n->grow();
    delete new_n;
  }
}
PICOBENCH(node_16_grow);

static void node_16_next_partial_key(state &s) {
  node_16<void *> n;
  node_0<void *> child;
  for (int i = 0; i < 16; ++i) {
    n.set_child((i * 17) - 128, &child);
  }
  for (auto _ : s) {
    auto next_partial_key = n.next_partial_key((rand() % 256) - 128);
  }
}
PICOBENCH(node_16_next_partial_key);

static void node_16_prev_partial_key(state &s) {
  node_16<void *> n;
  node_0<void *> child;
  for (int i = 0; i < 16; ++i) {
    n.set_child((i * 17) - 128, &child);
  }
  for (auto _ : s) {
    auto prev_partial_key = n.prev_partial_key((rand() % 256) - 128);
  }
}
PICOBENCH(node_16_prev_partial_key);
