/**
 * @file node_48 header
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_48_HPP
#define ART_NODE_48_HPP

#include "node.hpp"
#include <array>
#include <stdexcept>
#include <utility>

namespace art {

using std::array;

template <class T> class node_16;
template <class T> class node_256;

template <class T> class node_48 : public node<T> {
public:
  node_48();

  node<T> **find_child(uint8_t partial_key) override;
  void set_child(uint8_t partial_key, node<T> *child) override;
  node<T> *del_child(uint8_t partial_key) override;
  node<T> *grow() override;
  node<T> *shrink() override;
  bool is_full() const override;
  bool is_underfull() const override;

  uint8_t next_partial_key(uint8_t partial_key) const override;
  uint8_t prev_partial_key(uint8_t partial_key) const override;

  int n_children() const override;

private:
  static const uint8_t EMPTY;

  uint8_t n_children_ = 0;
  array<uint8_t, 256> indexes_;
  array<node<T> *, 48> children_;
};

template <class T> node_48<T>::node_48() {
  indexes_.fill(node_48::EMPTY);
  children_.fill(nullptr);
}

template <class T> node<T> **node_48<T>::find_child(uint8_t partial_key) {
  // TODO(rafaelkallis): direct lookup instead of temp save?
  uint8_t index = indexes_[partial_key];
  return node_48::EMPTY != index ? &children_[index] : nullptr;
}

template <class T>
void node_48<T>::set_child(uint8_t partial_key, node<T> *child) {

  // TODO(rafaelkallis): pick random starting entry in order to increase
  // performance? i.e. for (int i = random([0,48)); i != (i-1) % 48; i = (i+1) %
  // 48){}

  /* find empty child entry */
  for (int i = 0; i < 48; ++i) {
    if (nullptr == children_[i]) {
      indexes_[partial_key] = i;
      children_[i] = child;
      break;
    }
  }
  ++n_children_;
}

template <class T> node<T> *node_48<T>::del_child(uint8_t partial_key) {
  node<T> *child_to_delete = nullptr;
  uint8_t index = indexes_[partial_key];
  if (index != node_48::EMPTY) {
    indexes_[partial_key] = node_48::EMPTY;
    child_to_delete = children_[index];
    children_[index] = nullptr;
    --n_children_;
  }
  return child_to_delete;
}

template <class T> node<T> *node_48<T>::grow() {
  auto new_node = new node_256<T>();
  new_node->prefix_ = this->prefix_;
  new_node->prefix_len_ = this->prefix_len_;
  new_node->value_ = this->value_;
  for (int partial_key = 0; partial_key < 256; ++partial_key) {
    uint8_t index = indexes_[partial_key];
    if (index != node_48::EMPTY) {
      new_node->set_child(partial_key, children_[index]);
    }
  }
  delete this;
  return new_node;
}

template <class T> node<T> *node_48<T>::shrink() {
  auto new_node = new node_16<T>();
  new_node->prefix_ = this->prefix_;
  new_node->prefix_len_ = this->prefix_len_;
  new_node->value_ = this->value_;
  for (int partial_key = 0; partial_key < 256; ++partial_key) {
    uint8_t index = indexes_[partial_key];
    if (index != node_48::EMPTY) {
      new_node->set_child(partial_key, children_[index]);
    }
  }
  delete this;
  return new_node;
}

template <class T> bool node_48<T>::is_full() const {
  return n_children_ == 48;
}

template <class T> bool node_48<T>::is_underfull() const {
  return n_children_ == 16;
}

template <class T> const uint8_t node_48<T>::EMPTY = 48;

template <class T>
uint8_t node_48<T>::next_partial_key(uint8_t partial_key) const {
  for (int i = partial_key;; ++i) {
    if (indexes_[i] != node_48<T>::EMPTY) {
      return i;
    }
    if (i == 255) {
      throw std::out_of_range("provided partial key does not have a successor");
    }
  }
}

template <class T>
uint8_t node_48<T>::prev_partial_key(uint8_t partial_key) const {
  for (int i = partial_key;; --i) {
    if (indexes_[i] != node_48<T>::EMPTY) {
      return i;
    }
    if (i == 0) {
      throw std::out_of_range(
          "provided partial key does not have a predecessor");
    }
  }
}

template <class T> int node_48<T>::n_children() const { return n_children_; }

} // namespace art

#endif
