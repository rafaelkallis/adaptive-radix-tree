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

template <class T> class node_16;
template <class T> class node_256;

template <class T> class node_48 : public node<T> {
public:
  node_48();
  node_48(const string &key, const T &value);

  node<T> **find_child(partial_key_type partial_key) override;
  void set_child(partial_key_type partial_key, node<T> *child) override;
  node<T> *del_child(partial_key_type partial_key) override;
  node<T> *grow() override;
  node<T> *shrink() override;
  bool is_full() const override;
  bool is_underfull() const override;

  partial_key_type
  next_partial_key(partial_key_type partial_key) const override;

  partial_key_type
  prev_partial_key(partial_key_type partial_key) const override;

  partial_key_type indexes_[256];
  node<T> *children_[48];

private:
  static const partial_key_type EMPTY;
};

template <class T> node_48<T>::node_48() : node<T>() {
  for (int i = 0; i < 256; ++i) {
    this->indexes_[i] = node_48::EMPTY;
    if (i < 48) {
      this->children_[i] = nullptr;
    }
  }
}

template <class T>
node_48<T>::node_48(const string &key, const T &value)
    : node<T>(key, value), node_48<T>() {}

template <class T>
node<T> **node_48<T>::find_child(partial_key_type partial_key) {
  unsigned char index = this->indexes_[partial_key];
  return node_48::EMPTY != index ? &this->children_[index] : nullptr;
}

template <class T>
void node_48<T>::set_child(partial_key_type partial_key, node<T> *child) {

  // TODO(rafaelkallis): pick random starting entry in order to increase
  // performance? i.e. for (int i = random([0,48)); i != (i-1) % 48; i = (i+1) %
  // 48){}

  /* find empty child entry */
  for (int i = 0; i < 48; ++i) {
    if (nullptr == this->children_[i]) {
      this->indexes_[partial_key] = i;
      this->children_[i] = child;
      break;
    }
  }
  ++this->n_children_;
}

template <class T>
node<T> *node_48<T>::del_child(partial_key_type partial_key) {
  node<T> *child_to_delete = nullptr;
  uint8_t index = indexes_[partial_key];
  if (index != node_48::EMPTY) {
    indexes_[partial_key] = node_48::EMPTY;
    child_to_delete = children_[index];
    children_[index] = nullptr;
    --this->n_children_;
  }
  return child_to_delete;
}

template <class T> node<T> *node_48<T>::grow() {
  auto new_node = new node_256<T>();
  new_node.prefix_ = this->prefix_;
  new_node.prefix_len_ = this->prefix_len_;
  new_node.value_ = this->value_;
  for (int partial_key = 0; partial_key < 256; ++partial_key) {
    partial_key_type index = this->indexes_[partial_key];
    if (index != node_48::EMPTY) {
      new_node->set_child(partial_key, this->children_[index]);
    }
  }
  delete this;
  return new_node;
}

template <class T> node<T> *node_48<T>::shrink() {
  auto new_node = new node_16<T>();
  new_node.prefix_ = this->prefix_;
  new_node.prefix_len_ = this->prefix_len_;
  new_node.value_ = this->value_;
  for (int partial_key = 0; partial_key < 256; ++partial_key) {
    partial_key_type index = this->indexes_[partial_key];
    if (index != node_48::EMPTY) {
      new_node->set_child(partial_key, this->children_[index]);
    }
  }
  delete this;
  return new_node;
}

template <class T> bool node_48<T>::is_full() const {
  return this->n_children_ == 48;
}

template <class T> bool node_48<T>::is_underfull() const {
  return this->n_children_ == 16;
}

template <class T> const partial_key_type node_48<T>::EMPTY = 48;

template <class T>
partial_key_type
node_48<T>::next_partial_key(partial_key_type partial_key) const {
  for (partial_key_type i = partial_key;; ++i) {
    if (this->indexes_[i] != node_48<T>::EMPTY) {
      return i;
    }
    if (i == 255) {
      throw std::out_of_range("provided partial key does not have a successor");
    }
  }
}

template <class T>
partial_key_type
node_48<T>::prev_partial_key(partial_key_type partial_key) const {
  for (partial_key_type i = partial_key;; --i) {
    if (this->indexes_[i] != node_48<T>::EMPTY) {
      return i;
    }
    if (i == 0) {
      throw std::out_of_range(
          "provided partial key does not have a predecessor");
    }
  }
}

} // namespace art

#endif
