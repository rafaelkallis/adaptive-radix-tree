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

using std::array;
using std::out_of_range;

template <class T> class node_256 : public node<T> {
public:
  node_256();
  node_256(key_type prefix, T *value);

  node<T> **find_child(const partial_key_type &partial_key) override;
  void set_child(const partial_key_type &partial_key, node<T> *child) override;
  node<T> *grow() override;
  bool is_full() const override;
  bool is_leaf() const override;

  partial_key_type next_partial_key(
      partial_key_type partial_key) const noexcept(false) override;
  
  partial_key_type prev_partial_key(
      partial_key_type partial_key) const noexcept(false) override;

  int get_n_children() const override;

private:
  uint16_t n_children_;
  array<node<T> *, 256> children_;
};

template <class T>
node_256<T>::node_256() : node_256<T>(key_type(0), nullptr) {}

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
  this->n_children_ += 1;
}

template <class T> node<T> *node_256<T>::grow() { return this; }

template <class T> bool node_256<T>::is_full() const {
  return this->n_children_ == 256;
}

template <class T> bool node_256<T>::is_leaf() const {
  return this->n_children_ == 0;
}

template <class T>
partial_key_type node_256<T>::next_partial_key(
    partial_key_type partial_key) const noexcept(false) {
  for (partial_key_type i = partial_key;; ++i) {
    if (this->children_[i] != nullptr) {
      return i;
    }
    if (i == 255) {
      throw out_of_range("provided partial key does not have a successor");
    }
  }
}

template <class T>
partial_key_type node_256<T>::prev_partial_key(
    partial_key_type partial_key) const noexcept(false) {
  for (partial_key_type i = partial_key;; --i) {
    if (this->children_[i] != nullptr) {
      return i;
    }
    if (i == 0) {
      throw out_of_range("provided partial key does not have a predecessor");
    }
  }
}

template <class T> int node_256<T>::get_n_children() const {
  return this->n_children_;
}

} // namespace art

#endif
