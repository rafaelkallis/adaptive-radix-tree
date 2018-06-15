/**
 * @file node_256 header
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_256_HPP
#define ART_NODE_256_HPP

#include "node.hpp"
#include <array>
#include <utility>

namespace art {

using std::array;
using std::make_pair;
using std::pair;

template <class T> class node_256 : public node<T> {
public:
  node_256();
  node_256(key_type prefix, T *value);
  node_256(const node_256<T> &other) = default;
  node_256(node_256<T> &&other) noexcept = default;
  ~node_256() override = default;

  node_256<T> &operator=(const node_256<T> &other) = default;
  node_256<T> &operator=(node_256<T> &&other) noexcept = default;

  node<T> **find_child(const partial_key_type &partial_key) override;
  void set_child(const partial_key_type &partial_key, node<T> *child) override;
  node<T> *grow() override;
  bool is_full() const override;
  bool is_leaf() const override;

  typename node<T>::iterator begin() override;
  typename node<T>::iterator end() override;

  class iterator : public node<T>::iterator {
  public:
    iterator(node_256<T> *node, uint8_t index, bool is_out_of_bounds);

    pair<partial_key_type, node<T> *> operator*();
    iterator &operator++();
    bool operator==(const node_256<T>::iterator &rhs);
    bool operator!=(const node_256<T>::iterator &rhs);

  private:
    node_256<T> *node_;
    uint8_t index_;
    bool is_out_of_bounds_;
  };

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

template <class T> typename node<T>::iterator node_256<T>::begin() {
  return node_256<T>::iterator(this, 0, false);
}

template <class T> typename node<T>::iterator node_256<T>::end() {
  return node_256<T>::iterator(this, 0, true);
}

template <class T>
node_256<T>::iterator::iterator(node_256<T> *node, uint8_t index, bool is_out_of_bounds)
    : node_(node), index_(index), is_out_of_bounds_(is_out_of_bounds) {}

template <class T>
pair<partial_key_type, node<T> *> node_256<T>::iterator::operator*() {
  if (this->is_out_of_bounds_) {
    return make_pair(0, nullptr);
  }
  return make_pair(this->index_,
                   this->node_->children_[this->index_]);
}

template <class T>
typename node_256<T>::iterator &node_256<T>::iterator::operator++() {
  if (!this->is_out_of_bounds_) {
    /* advance iterator index until non-nullptr child is found */
    do {
      if (this->index_ == 255) {
        this->is_out_of_bounds_ = true;
        break;
      }
      this->index_ += 1;
    } while (this->node_->children_[this->index_] == nullptr);
  }
  return *this;
}

template <class T>
bool node_256<T>::iterator::operator==(const node_256<T>::iterator &rhs) {
  return this->node_ == rhs.node_ &&
         ((this->is_out_of_bounds_ && rhs.is_out_of_bounds_) ||
          (!this->is_out_of_bounds_ && !rhs.is_out_of_bounds_ &&
           this->index_ == rhs.index_));
}

template <class T>
bool node_256<T>::iterator::operator!=(const node_256<T>::iterator &rhs) {
  return !((*this) == rhs);
}

} // namespace art

#endif
