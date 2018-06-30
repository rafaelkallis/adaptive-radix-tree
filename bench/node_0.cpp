/**
 * @file node_0 benchmarks
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "picobench/picobench.hpp"

using namespace art;
using picobench::state;

PICOBENCH_SUITE("node_0");

static void node_0_constructor(state &s) {
  for (auto _ : s) {
    node_0<void *> n;
  }
}
PICOBENCH(node_0_constructor);

static void node_0_grow(state &s) {
  node<void *> *n = nullptr;
  node<void *> *new_n = nullptr;
  for (auto _ : s) {
    n = new node_0<void *>();
    new_n = n->grow();
    delete new_n;
  }
}
PICOBENCH(node_0_grow);
