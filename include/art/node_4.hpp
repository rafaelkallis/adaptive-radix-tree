/**
 * @file node_4 header
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_4_HPP
#define ART_NODE_4_HPP

#include "node.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace art {

template <class T> class node_0;
template <class T> class node_16;

template <class T> class node_4 : public node<T> {
public:
  node_4();
  node_4(const key_type &prefix, T *value);

  node<T> **find_child(const partial_key_type &partial_key) override;
  void set_child(const partial_key_type &partial_key, node<T> *child) override;
  node<T> *del_child(const partial_key_type &partial_key) override;
  node<T> *grow() override;
  node<T> *shrink() override;
  bool is_full() const override;
  bool is_underfull() const override;

  partial_key_type
  next_partial_key(partial_key_type partial_key) const override;

  partial_key_type
  prev_partial_key(partial_key_type partial_key) const override;

  int get_n_children() const override;

private:
  uint8_t n_children_;
  std::array<partial_key_type, 4> keys_;
  std::array<node<T> *, 4> children_;
};

template <class T> node_4<T>::node_4() : node_4<T>(key_type(), nullptr) {}

template <class T>
node_4<T>::node_4(const key_type &prefix, T *value)
    : node<T>(prefix, value), n_children_(0), keys_(), children_() {}

template <class T>
node<T> **node_4<T>::find_child(const partial_key_type &partial_key) {
  for (int i = 0; i < n_children_; ++i) {
    if (keys_[i] == partial_key) {
      return &children_[i];
    }
  }
  return nullptr;
}

template <class T>
void node_4<T>::set_child(const partial_key_type &partial_key, node<T> *child) {
  /* determine index for child */
  int child_i;
  for (int i = this->n_children_ - 1;; --i) {
    if (i >= 0 && partial_key < this->keys_[i]) {
      /* move existing sibling to the right */
      this->keys_[i + 1] = this->keys_[i];
      this->children_[i + 1] = this->children_[i];
    } else {
      child_i = i + 1;
      break;
    }
  }

  this->keys_[child_i] = partial_key;
  this->children_[child_i] = child;
  ++n_children_;
}

template <class T>
node<T> *node_4<T>::del_child(const partial_key_type &partial_key) {
  node<T> *child_to_delete = nullptr;
  for (int i = 0; i < n_children_; ++i) {
    if (child_to_delete == nullptr && partial_key == keys_[i]) {
      child_to_delete = children_[i];
    }
    if (child_to_delete != nullptr) {
      /* move existing sibling to the left */
      keys_[i] = i < n_children_ - 1 ? keys_[i + 1] : 0;
      children_[i] = i < n_children_ - 1 ? children_[i + 1] : nullptr;
    }
  }
  if (child_to_delete != nullptr) {
    --n_children_;
  }
  return child_to_delete;
}

template <class T> node<T> *node_4<T>::grow() {
  auto new_node = new node_16<T>(this->get_prefix(), this->get_value());
  for (int i = 0; i < n_children_; ++i) {
    new_node->set_child(keys_[i], children_[i]);
  }
  delete this;
  return new_node;
}

template <class T> node<T> *node_4<T>::shrink() {
  auto new_node = new node_0<T>(this->get_prefix(), this->get_value());
  delete this;
  return new_node;
}

template <class T> bool node_4<T>::is_full() const {
  return this->n_children_ == 4;
}

template <class T> bool node_4<T>::is_underfull() const {
  return this->n_children_ == 0;
}

template <class T>
partial_key_type
node_4<T>::next_partial_key(partial_key_type partial_key) const {
  for (int i = 0; i < n_children_; ++i) {
    if (keys_[i] >= partial_key) {
      return keys_[i];
    }
  }
  /* return 0; */
  throw std::out_of_range("provided partial key does not have a successor");
}

template <class T>
partial_key_type
node_4<T>::prev_partial_key(partial_key_type partial_key) const {
  for (int i = this->n_children_ - 1; i >= 0; --i) {
    if (this->keys_[i] <= partial_key) {
      return this->keys_[i];
    }
  }
  /* return 255; */
  throw std::out_of_range("provided partial key does not have a predecessor");
}

template <class T> int node_4<T>::get_n_children() const {
  return this->n_children_;
}

} // namespace art

#endif
