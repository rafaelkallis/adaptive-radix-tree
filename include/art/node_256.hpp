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

template <class T> class node_48;

template <class T> class node_256 : public node<T> {
public:
  node_256();
  node_256(key_type prefix, T *value);

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
  uint16_t n_children_;
  std::array<node<T> *, 256> children_;
};

template <class T>
node_256<T>::node_256() : node_256<T>(key_type(), nullptr) {}

template <class T>
node_256<T>::node_256(key_type prefix, T *value)
    : node<T>(prefix, value), n_children_(0), children_() {
  for (int i = 0; i < 256; i += 1) {
    this->children_[i] = nullptr;
  }
}

template <class T>
node<T> **node_256<T>::find_child(const partial_key_type &partial_key) {
  return this->children_[partial_key] != nullptr ? &this->children_[partial_key]
                                                 : nullptr;
}

template <class T>
void node_256<T>::set_child(const partial_key_type &partial_key,
                            node<T> *child) {
  this->children_[partial_key] = child;
  ++n_children_;
}

template <class T>
node<T> *node_256<T>::del_child(const partial_key_type &partial_key) {
  node<T> *child_to_delete = children_[partial_key];
  if (child_to_delete != nullptr) {
    children_[partial_key] = nullptr;
    --n_children_;
  }
  return child_to_delete;
}

template <class T> node<T> *node_256<T>::grow() {
  throw std::runtime_error("node_256 cannot grow");
}

template <class T> node<T> *node_256<T>::shrink() {
  auto new_node = new node_48<T>(this->get_prefix(), this->get_value());
  for (int partial_key = 0; partial_key < 256; ++partial_key) {
    if (this->children_[partial_key] != nullptr) {
      new_node->set_child(partial_key, this->children_[partial_key]);
    }
  }
  delete this;
  return new_node;
}

template <class T> bool node_256<T>::is_full() const {
  return this->n_children_ == 256;
}

template <class T> bool node_256<T>::is_underfull() const {
  return this->n_children_ == 48;
}

template <class T>
partial_key_type
node_256<T>::next_partial_key(partial_key_type partial_key) const {
  for (partial_key_type i = partial_key;; ++i) {
    if (this->children_[i] != nullptr) {
      return i;
    }
    if (i == 255) {
      throw std::out_of_range("provided partial key does not have a successor");
    }
  }
}

template <class T>
partial_key_type
node_256<T>::prev_partial_key(partial_key_type partial_key) const {
  for (partial_key_type i = partial_key;; --i) {
    if (this->children_[i] != nullptr) {
      return i;
    }
    if (i == 0) {
      throw std::out_of_range(
          "provided partial key does not have a predecessor");
    }
  }
}

template <class T> int node_256<T>::get_n_children() const {
  return this->n_children_;
}

} // namespace art

#endif
