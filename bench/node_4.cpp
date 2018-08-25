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
  for (auto _ : s) {
    node_4<void *> n;
  }
}
PICOBENCH(node_4_constructor);

static void node_4_find_child(state &s) {
  node_4<void *> n;

  node_0<void *> c0;
  node_0<void *> c1;
  node_0<void *> c2;
  node_0<void *> c3;

  n.set_child(-128, &c0);
  n.set_child(-43, &c1);
  n.set_child(42, &c2);
  n.set_child(127, &c3);

  char partial_keys[] = {-128, -43, 42, 127};
  node<void *> **child = nullptr;
  for (auto _ : s) {
    child = n.find_child(partial_keys[rand() % 4]);
  }
}
PICOBENCH(node_4_find_child);

static void node_4_set_child(state &s) {
  node_4<void *> *n = new node_4<void *>();
  node_0<void *> child;
  char partial_keys[] = {-128, -43, 42, 127};
  for (auto _ : s) {
    if (n->is_full()) {
      delete n;
      n = new node_4<void *>();
    }

    n->set_child(partial_keys[rand() % 4], &child);
  }
}
PICOBENCH(node_4_set_child);

static void node_4_grow(state &s) {
  node<void *> *n = nullptr;
  node<void *> *new_n = nullptr;
  for (auto _ : s) {
    n = new node_4<void *>();
    new_n = n->grow();
    delete new_n;
  }
}
PICOBENCH(node_4_grow);

static void node_4_next_partial_key(state &s) {
  node_4<void *> n;

  node_0<void *> c0;
  node_0<void *> c1;
  node_0<void *> c2;
  node_0<void *> c3;

  n.set_child(-128, &c0);
  n.set_child(-43, &c1);
  n.set_child(42, &c2);
  n.set_child(127, &c3);

  for (auto _ : s) {
    auto next_partial_key = n.next_partial_key((rand() % 256) - 128);
  }
}
PICOBENCH(node_4_next_partial_key);

static void node_4_prev_partial_key(state &s) {
  node_4<void *> n;

  node_0<void *> c0;
  node_0<void *> c1;
  node_0<void *> c2;
  node_0<void *> c3;

  n.set_child(-128, &c0);
  n.set_child(-43, &c1);
  n.set_child(42, &c2);
  n.set_child(127, &c3);

  for (auto _ : s) {
    auto prev_partial_key = n.prev_partial_key((rand() % 256) - 128);
  }
}
PICOBENCH(node_4_prev_partial_key);
