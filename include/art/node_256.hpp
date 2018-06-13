/**
 * @file node_256 header
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_256_HPP
#define ART_NODE_256_HPP

#include "node.hpp"
#include "node_48.hpp"

// TODO(rafaelkallis): rule of five

template <class T> class node_256 : public node<T> {
public:
  node_256();
  node_256(key_type prefix, T *value);
  ~node_256() override = default;

  node<T> *find_child(const partial_key_type &partial_key) const override;
  void set_child(const partial_key_type &partial_key, node<T> *child) override;
  node<T> *grow() override;
  bool is_full() const override;
  bool is_leaf() const override;

private:
  uint16_t n_children_;
  array<node<T> *, 256> children_;
};

template <class T>
node_256<T>::node_256() : node_256<T>(key_type(0), nullptr) {}

template <class T>
node_256<T>::node_256(key_type prefix, T *value)
    : node<T>(prefix, value), n_children_(0) {
  for (int i = 0; i < 256; i += 1) {
    this->children_[i] = nullptr;
  }
}

template <class T>
node<T> *node_256<T>::find_child(const partial_key_type &partial_key) const {
  return this->children_[partial_key];
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
#endif
