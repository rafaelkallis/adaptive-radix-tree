/**
 * @file node_256 benchmarks
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "picobench/picobench.hpp"

using namespace art;
using picobench::state;

PICOBENCH_SUITE("node_256");

static void node_256_constructor(state &s) {
  for (auto _ : s) {
    node_256<void *> n;
  }
}
PICOBENCH(node_256_constructor);

static void node_256_find_child(state &s) {
  node_256<void *> n;
  node_0<void *> child;
  for (int i = 0; i < 256; ++i) {
    n.set_child(i - 128, &child);
  }
  for (auto _ : s) {
    auto child = n.find_child((rand() % 256) - 128);
  }
}
PICOBENCH(node_256_find_child);

static void node_256_set_child(state &s) {
  node_256<void *> *n = new node_256<void *>();
  node_0<void *> child;
  for (auto _ : s) {
    if (n->is_full()) {
      delete n;
      n = new node_256<void *>();
    }
    n->set_child((rand() % 256) - 128, &child);
  }
}
PICOBENCH(node_256_set_child);

static void node_256_next_partial_key(state &s) {
  node_256<void *> n;
  node_0<void *> child;
  for (int i = 0; i < 256; ++i) {
    n.set_child(i - 128, &child);
  }
  for (auto _ : s) {
    auto next_partial_key = n.next_partial_key((rand() % 256) - 128);
  }
}
PICOBENCH(node_256_next_partial_key);

static void node_256_prev_partial_key(state &s) {
  node_256<void *> n;
  node_0<void *> child;
  for (int i = 0; i < 256; ++i) {
    n.set_child(i - 128, &child);
  }
  for (auto _ : s) {
    auto prev_partial_key = n.prev_partial_key((rand() % 256) - 128);
  }
}
PICOBENCH(node_256_prev_partial_key);
