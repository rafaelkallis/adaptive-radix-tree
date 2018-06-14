/**
 * @file node_16 header
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_16_HPP
#define ART_NODE_16_HPP

#include "node.hpp"
#include "node_48.hpp"

namespace art {

template <class T> class node_16 : public node<T> {
public:
  node_16();
  node_16(key_type prefix, T *value);
  node_16(const node_16<T> &other) = default;
  node_16(node_16<T> &&other) noexcept = default;
  ~node_16() override = default;

  node_16<T> &operator=(const node_16<T> &other) = default;
  node_16<T> &operator=(node_16<T> &&other) noexcept = default;

  node<T> **find_child(const partial_key_type &partial_key) override;
  void set_child(const partial_key_type &partial_key, node<T> *child) override;
  node<T> *grow() override;
  bool is_full() const override;
  bool is_leaf() const override;

private:
  uint8_t n_children_;
  array<partial_key_type, 16> keys_;
  array<node<T> *, 16> children_;
};

template <class T> node_16<T>::node_16() : node_16<T>(key_type(0), nullptr) {}

template <class T>
node_16<T>::node_16(key_type prefix, T *value)
    : node<T>(prefix, value), n_children_(0), keys_() {}

template <class T>
node<T> **node_16<T>::find_child(const partial_key_type &partial_key) {
  for (int i = 0; i < this->n_children_; i += 1) {
    if (this->keys_[i] == partial_key) {
      return &this->children_[i];
    }
  }
  return nullptr;
}

template <class T>
void node_16<T>::set_child(const partial_key_type &partial_key,
                           node<T> *child) {
  /* determine index for child */
  int child_i;
  for (int i = this->n_children_ - 1;; i -= 1) {
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
  this->n_children_ += 1;
}

template <class T> node<T> *node_16<T>::grow() {
  auto new_node = new node_48<T>(this->get_prefix(), this->get_value());
  for (int i = 0; i < this->n_children_; i += 1) {
    new_node->set_child(this->keys_[i], this->children_[i]);
  }
  delete this;
  return new_node;
}

template <class T> bool node_16<T>::is_full() const {
  return this->n_children_ == 16;
}

template <class T> bool node_16<T>::is_leaf() const {
  return this->n_children_ == 0;
}

} // namespace art

#endif
