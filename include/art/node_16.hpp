/**
 * @file node_16 header
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_16_HPP
#define ART_NODE_16_HPP

#include "node.hpp"
#include "node_48.hpp"
#include <utility>

namespace art {

using std::make_pair;
using std::pair;

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
  
  typename node<T>::iterator begin() override;
  typename node<T>::iterator end() override;

  class iterator : public node<T>::iterator {
  public:
    iterator(node_16<T> *node, uint8_t index);

    pair<partial_key_type, node<T> *> operator*();
    iterator &operator++();
    bool operator==(const node_16<T>::iterator &rhs);
    bool operator!=(const node_16<T>::iterator &rhs);

  private:
    node_16<T> *node_;
    uint8_t index_;
  };

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

template <class T> typename node<T>::iterator node_16<T>::begin() {
  return node_16<T>::iterator(this, 0);
}

template <class T> typename node<T>::iterator node_16<T>::end() {
  return node_16<T>::iterator(this, this->n_children_);
}

template <class T>
node_16<T>::iterator::iterator(node_16<T> *node, uint8_t index)
    : node_(node), index_(index) {}

template <class T>
pair<partial_key_type, node<T> *> node_16<T>::iterator::operator*() {
  if (this->index_ >= this->node_->n_children_) {
    /* out of bounds */
    return make_pair(0, nullptr);
  }
  return make_pair(this->node_->keys_[this->index_],
                   this->node_->children_[this->index_]);
}

template <class T>
typename node_16<T>::iterator &node_16<T>::iterator::operator++() {
  if (this->index_ < this->n_children_) {
    this->index_++;
  }
  return *this;
}

template <class T>
bool node_16<T>::iterator::operator==(const node_16<T>::iterator &rhs) {
  return this->node_ == rhs.node_ && this->index_ == rhs.index_;
}

template <class T>
bool node_16<T>::iterator::operator!=(const node_16<T>::iterator &rhs) {
  return !((*this) == rhs);
}

} // namespace art

#endif
