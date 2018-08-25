/**
 * @file node_256 header
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_256_HPP
#define ART_NODE_256_HPP

#include "node.hpp"
#include <array>
#include <stdexcept>

namespace art {

using std::out_of_range;
using std::runtime_error;

template <class T> class node_48;

template <class T> class node_256 : public node<T> {
public:
  node_256();

  node<T> **find_child(char partial_key) override;
  void set_child(char partial_key, node<T> *child) override;
  node<T> *del_child(char partial_key) override;
  node<T> *grow() override;
  node<T> *shrink() override;
  bool is_full() const override;
  bool is_underfull() const override;

  char next_partial_key(char partial_key) const override;

  char prev_partial_key(char partial_key) const override;

  int n_children() const override;

private:
  uint16_t n_children_ = 0;
  std::array<node<T> *, 256> children_;
};

template <class T> node_256<T>::node_256() {
  children_.fill(nullptr);
}

template <class T> node<T> **node_256<T>::find_child(char partial_key) {
  return children_[128 + partial_key] != nullptr ? &children_[128 + partial_key] : nullptr;
}

template <class T>
void node_256<T>::set_child(char partial_key, node<T> *child) {
  children_[128 + partial_key] = child;
  ++n_children_;
}

template <class T> node<T> *node_256<T>::del_child(char partial_key) {
  node<T> *child_to_delete = children_[128 + partial_key];
  if (child_to_delete != nullptr) {
    children_[128 + partial_key] = nullptr;
    --n_children_;
  }
  return child_to_delete;
}

template <class T> node<T> *node_256<T>::grow() {
  throw runtime_error("node_256 cannot grow");
}

template <class T> node<T> *node_256<T>::shrink() {
  auto new_node = new node_48<T>();
  new_node->prefix_ = this->prefix_;
  new_node->prefix_len_ = this->prefix_len_;
  new_node->value_ = this->value_;
  for (int partial_key = 0; partial_key < 256; ++partial_key) {
    if (children_[128 + partial_key] != nullptr) {
      new_node->set_child(partial_key, children_[128 + partial_key]);
    }
  }
  delete this;
  return new_node;
}

template <class T> bool node_256<T>::is_full() const {
  return n_children_ == 256;
}

template <class T> bool node_256<T>::is_underfull() const {
  return n_children_ == 48;
}

template <class T> char node_256<T>::next_partial_key(char partial_key) const {
  while (true) {
    if (children_[128 + partial_key] != nullptr) {
      return partial_key;
    }
    if (partial_key == 127) {
      throw out_of_range("provided partial key does not have a successor");
    }
    ++partial_key;
  }
}

template <class T> char node_256<T>::prev_partial_key(char partial_key) const {
  while (true) {
    if (children_[128 + partial_key] != nullptr) {
      return partial_key;
    }
    if (partial_key == -128) {
      throw out_of_range("provided partial key does not have a predecessor");
    }
    --partial_key;
  }
}

template <class T> int node_256<T>::n_children() const { return n_children_; }

} // namespace art

#endif
