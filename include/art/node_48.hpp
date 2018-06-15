/**
 * @file node_48 header
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_48_HPP
#define ART_NODE_48_HPP

#include "node.hpp"
#include "node_256.hpp"
#include <array>
#include <utility>

namespace art {

using std::array;
using std::make_pair;
using std::pair;

template <class T> class node_48 : public node<T> {
public:
  node_48();
  node_48(key_type prefix, T *value);
  node_48(const node_48<T> &other) = default;
  node_48(node_48<T> &&other) noexcept = default;
  ~node_48() override = default;

  node_48<T> &operator=(const node_48<T> &other) = default;
  node_48<T> &operator=(node_48<T> &&other) noexcept = default;

  node<T> **find_child(const partial_key_type &partial_key) override;
  void set_child(const partial_key_type &partial_key, node<T> *child) override;
  node<T> *grow() override;
  bool is_full() const override;
  bool is_leaf() const override;

  typename node<T>::iterator begin() override;
  typename node<T>::iterator end() override;

  class iterator : public node<T>::iterator {
  public:
    iterator(node_48<T> *node, uint8_t index, bool is_out_of_bounds);

    pair<partial_key_type, node<T> *> operator*();
    iterator &operator++();
    bool operator==(const node_48<T>::iterator &rhs);
    bool operator!=(const node_48<T>::iterator &rhs);

  private:
    node_48<T> *node_;
    uint8_t index_;
    bool is_out_of_bounds_;
  };

private:
  static const partial_key_type EMPTY;

  uint8_t n_children_;
  array<partial_key_type, 256> indexes_;
  array<node<T> *, 48> children_;
};

template <class T> node_48<T>::node_48() : node_48<T>(key_type(0), nullptr) {}

template <class T>
node_48<T>::node_48(key_type prefix, T *value)
    : node<T>(prefix, value), n_children_(0), indexes_(), children_() {
  for (int i = 0; i < 256; i += 1) {
    this->indexes_[i] = node_48::EMPTY;
    if (i < 48) {
      this->children_[i] = nullptr;
    }
  }
}

template <class T>
node<T> **node_48<T>::find_child(const partial_key_type &partial_key) {
  unsigned char index = this->indexes_[partial_key];
  return node_48::EMPTY != index ? &this->children_[index] : nullptr;
}

template <class T>
void node_48<T>::set_child(const partial_key_type &partial_key,
                           node<T> *child) {

  // TODO(rafaelkallis): pick random starting entry in order to increase
  // performance? i.e. for (int i = random([0,48)); i != (i-1) % 48; i = (i+1) %
  // 48){}

  /* find empty child entry */
  for (int i = 0; i < 48; i += 1) {
    if (nullptr == this->children_[i]) {
      this->indexes_[partial_key] = i;
      this->children_[i] = child;
      break;
    }
  }
  this->n_children_ += 1;
}

template <class T> node<T> *node_48<T>::grow() {
  auto new_node = new node_256<T>(this->get_prefix(), this->get_value());
  for (int partial_key = 0; partial_key < 256; partial_key += 1) {
    unsigned char index = this->indexes_[partial_key];
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

template <class T> bool node_48<T>::is_leaf() const {
  return this->n_children_ == 0;
}

template <class T> const unsigned char node_48<T>::EMPTY = 48;

template <class T> typename node<T>::iterator node_48<T>::begin() {
  return node_48<T>::iterator(this, 0, false);
}

template <class T> typename node<T>::iterator node_48<T>::end() {
  return node_48<T>::iterator(this, 0, true);
}

template <class T>
node_48<T>::iterator::iterator(node_48<T> *node, uint8_t index, bool is_out_of_bounds)
    : node_(node), index_(index), is_out_of_bounds_(is_out_of_bounds) {}

template <class T>
pair<partial_key_type, node<T> *> node_48<T>::iterator::operator*() {
  if (this->is_out_of_bounds_) {
    return make_pair(0, nullptr);
  }
  return make_pair(this->index_,
                   this->node_->children_[this->node_->indexes_[this->index_]]);
}

template <class T>
typename node_48<T>::iterator &node_48<T>::iterator::operator++() {
  if (!this->is_out_of_bounds_) {
    /* advance iterator index until non-empty child index is found */
    do {
      if (this->index_ == 255) {
        this->is_out_of_bounds_ = true;
        break;
      }
      this->index_ += 1;
    } while (this->node_->indexes_[this->index_] == node_48<T>::EMPTY);
  }
  return *this;
}

template <class T>
bool node_48<T>::iterator::operator==(const node_48<T>::iterator &rhs) {
  return this->node_ == rhs.node_ &&
         ((this->is_out_of_bounds_ && rhs.is_out_of_bounds_) ||
          (!this->is_out_of_bounds_ && !rhs.is_out_of_bounds_ &&
           this->index_ == rhs.index_));
}

template <class T>
bool node_48<T>::iterator::operator!=(const node_48<T>::iterator &rhs) {
  return !((*this) == rhs);
}

} // namespace art

#endif
