/**
 * @file child_iterator
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_CHILDREN_ITERATOR_HPP
#define ART_CHILDREN_ITERATOR_HPP

#include "constants.hpp"
#include <iterator>

namespace art {

template <class T> class node;

template <class T> class children_iterator {
public:
  children_iterator(node<T> *n, int relative_index);

  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = const partial_key_type;
  using difference_type = int;
  using pointer = value_type *;
  using reference = value_type &;

  reference operator*() const;
  pointer operator->() const;
  children_iterator &operator++();
  children_iterator operator++(int);
  children_iterator &operator--();
  children_iterator operator--(int);
  bool operator==(const children_iterator &rhs) const;
  bool operator!=(const children_iterator &rhs) const;
  bool operator<(const children_iterator &rhs) const;
  bool operator>(const children_iterator &rhs) const;
  bool operator<=(const children_iterator &rhs) const;
  bool operator>=(const children_iterator &rhs) const;

private:
  node<T> *node_;
  partial_key_type cur_partial_key_;
  int relative_index_;
};

template <class T>
children_iterator<T>::children_iterator(node<T> *n, int relative_index)
    : node_(n), cur_partial_key_(0), relative_index_(relative_index) {
  if (relative_index_ < 0) {
    /* relative_index is out of bounds, no seek */
    return;
  }
  if (relative_index_ >= node_->get_n_children()) {
    /* relative_index is out of bounds, no seek */
    return;
  }

  /* relative_index is in bounds, (forward) seek next partial key */
  for (int i = 0; i < relative_index_; ++i) {
    cur_partial_key_ = node_->next_partial_key(cur_partial_key_);
  }
}

template <class T>
typename children_iterator<T>::reference children_iterator<T>::
operator*() const {
  return cur_partial_key_;
}

template <class T>
typename children_iterator<T>::pointer children_iterator<T>::
operator->() const {
  return &cur_partial_key_;
}

template <class T> children_iterator<T> &children_iterator<T>::operator++() {
  ++relative_index_;
  if (relative_index_ == 0) {
    cur_partial_key_ = node_->next_partial_key(0);
  } else if (relative_index_ < node_->get_n_children()) {
    cur_partial_key_ = node_->next_partial_key(cur_partial_key_ + 1);
  }
  return *this;
}

template <class T> children_iterator<T> children_iterator<T>::operator++(int) {
  auto old = *this;
  operator++();
  return old;
}

template <class T> children_iterator<T> &children_iterator<T>::operator--() {
  --relative_index_;
  if (relative_index_ == node_->get_n_children() - 1) {
    cur_partial_key_ = node_->prev_partial_key(255);
  } else if (relative_index_ >= 0) {
    cur_partial_key_ = node_->prev_partial_key(cur_partial_key_ - 1);
  }
  return *this;
}

template <class T> children_iterator<T> children_iterator<T>::operator--(int) {
  auto old = *this;
  operator--();
  return old;
}

template <class T>
bool children_iterator<T>::operator==(const children_iterator<T> &rhs) const {
  /* should be from same node */
  assert(node_ == rhs.node_);
  return relative_index_ == rhs.relative_index_;
}

template <class T>
bool children_iterator<T>::operator<(const children_iterator<T> &rhs) const {
  /* should be from same node */
  assert(node_ == rhs.node_);
  return relative_index_ < rhs.relative_index_;
}

template <class T>
bool children_iterator<T>::operator!=(const children_iterator<T> &rhs) const {
  return !((*this) == rhs);
}

template <class T>
bool children_iterator<T>::operator>=(const children_iterator<T> &rhs) const {
  return !((*this) < rhs);
}

template <class T>
bool children_iterator<T>::operator<=(const children_iterator<T> &rhs) const {
  return (rhs >= (*this));
}

template <class T>
bool children_iterator<T>::operator>(const children_iterator<T> &rhs) const {
  return (rhs < (*this));
}

} // namespace art

#endif
