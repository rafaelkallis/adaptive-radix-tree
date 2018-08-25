/**
 * @file node_48 benchmarks
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "picobench/picobench.hpp"
#include <cmath>

using namespace art;
using picobench::state;

PICOBENCH_SUITE("node_48");

static void node_48_constructor(state &s) {
  for (auto _ : s) {
    node_48<void *> n;
  }
}
PICOBENCH(node_48_constructor);

static void node_48_find_child(state &s) {
  node_48<void *> n;
  node_0<void *> child;
  for (int i = 0; i < 48; ++i) {
    n.set_child(std::floor(5.4468 * i) - 128, &child);
  }
  for (auto _ : s) {
    auto child = n.find_child(std::floor(5.4468 * (rand() % 48)) - 128);
  }
}
PICOBENCH(node_48_find_child);

static void node_48_set_child(state &s) {
  node_48<void *> *n = new node_48<void *>();
  node_0<void *> child;
  for (auto _ : s) {
    if (n->is_full()) {
      delete n;
      n = new node_48<void *>();
    }
    n->set_child((rand() % 256) - 128, &child);
  }
}
PICOBENCH(node_48_set_child);

static void node_48_grow(state &s) {
  for (auto _ : s) {
    node_48<void *> *n = new node_48<void *>();
    node<void *> *new_n = n->grow();
    delete new_n;
  }
}
PICOBENCH(node_48_grow);

static void node_48_next_partial_key(state &s) {
  node_48<void *> n;
  node_0<void *> child;
  for (int i = 0; i < 48; ++i) {
    n.set_child(std::floor(5.4468 * i) - 128, &child);
  }
  for (auto _ : s) {
    auto next_partial_key = n.next_partial_key((rand() % 256) - 128);
  }
}
PICOBENCH(node_48_next_partial_key);

static void node_48_prev_partial_key(state &s) {
  node_48<void *> n;
  node_0<void *> child;
  for (int i = 0; i < 48; ++i) {
    n.set_child(std::floor(5.4468 * i) - 128, &child);
  }
  for (auto _ : s) {
    auto prev_partial_key = n.prev_partial_key((rand() % 256) - 128);
  }
}
PICOBENCH(node_48_prev_partial_key);
