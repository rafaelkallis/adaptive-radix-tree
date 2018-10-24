/**
 * @file node_48 header
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_48_HPP
#define ART_NODE_48_HPP

#include "node.hpp"
#include <algorithm>
#include <array>
#include <stdexcept>
#include <utility>

namespace art {

template <class T> class node_16;
template <class T> class node_256;

template <class T> class node_48 : public node<T> {
  friend class node_16<T>;
  friend class node_256<T>;

public:
  node_48();

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
  static const char EMPTY;

  uint8_t n_children_ = 0;
  char indexes_[256];
  node<T> *children_[48];
};

template <class T> node_48<T>::node_48() {
  std::fill(this->indexes_, this->indexes_ + 256,node_48::EMPTY);
  std::fill(this->children_, this->children_ + 48, nullptr);
}

template <class T> node<T> **node_48<T>::find_child(char partial_key) {
  // TODO(rafaelkallis): direct lookup instead of temp save?
  char index = indexes_[128 + partial_key];
  return node_48::EMPTY != index ? &children_[index] : nullptr;
}

template <class T>
void node_48<T>::set_child(char partial_key, node<T> *child) {

  // TODO(rafaelkallis): pick random starting entry in order to increase
  // performance? i.e. for (int i = random([0,48)); i != (i-1) % 48; i = (i+1) %
  // 48){}

  /* find empty child entry */
  for (int i = 0; i < 48; ++i) {
    if (children_[i] == nullptr) {
      indexes_[128 + partial_key] = i;
      children_[i] = child;
      break;
    }
  }
  ++n_children_;
}

template <class T> node<T> *node_48<T>::del_child(char partial_key) {
  node<T> *child_to_delete = nullptr;
  char index = indexes_[128 + partial_key];
  if (index != node_48::EMPTY) {
    child_to_delete = children_[index];
    indexes_[128 + partial_key] = node_48::EMPTY;
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
  char index;
  for (int partial_key = -128; partial_key < 127; ++partial_key) {
    index = indexes_[128 + partial_key];
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
  char index;
  for (int partial_key = -128; partial_key < 127; ++partial_key) {
    index = indexes_[128 + partial_key];
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

template <class T> const char node_48<T>::EMPTY = 48;

template <class T> char node_48<T>::next_partial_key(char partial_key) const {
  while (true) {
    if (indexes_[128 + partial_key] != node_48<T>::EMPTY) {
      return partial_key;
    }
    if (partial_key == 127) {
      throw std::out_of_range("provided partial key does not have a successor");
    }
    ++partial_key;
  }
}

template <class T> char node_48<T>::prev_partial_key(char partial_key) const {
  while (true) {
    if (indexes_[128 + partial_key] != node_48<T>::EMPTY) {
      return partial_key;
    }
    if (partial_key == -128) {
      throw std::out_of_range(
          "provided partial key does not have a predecessor");
    }
    --partial_key;
  }
}

template <class T> int node_48<T>::n_children() const { return n_children_; }

} // namespace art

#endif
