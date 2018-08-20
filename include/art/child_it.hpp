/**
 * @file child iterator
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_CHILD_IT_HPP
#define ART_CHILD_IT_HPP

#include "constants.hpp"
#include <iterator>

namespace art {

template <class T> class node;

template <class T> class child_it {
public:
  child_it(node<T> *n, int relative_index);

  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = const partial_key_type;
  using difference_type = int;
  using pointer = value_type *;
  using reference = value_type &;

  reference operator*() const;
  pointer operator->() const;
  child_it &operator++();
  child_it operator++(int);
  child_it &operator--();
  child_it operator--(int);
  bool operator==(const child_it &rhs) const;
  bool operator!=(const child_it &rhs) const;
  bool operator<(const child_it &rhs) const;
  bool operator>(const child_it &rhs) const;
  bool operator<=(const child_it &rhs) const;
  bool operator>=(const child_it &rhs) const;

private:
  node<T> *node_;
  uint8_t cur_partial_key_;
  int relative_index_;
};

template <class T>
child_it<T>::child_it(node<T> *n, int relative_index)
    : node_(n), cur_partial_key_(0), relative_index_(relative_index) {
  if (relative_index_ < 0) {
    /* relative_index is out of bounds, no seek */
    return;
  }
  if (relative_index_ >= node_->n_children()) {
    /* relative_index is out of bounds, no seek */
    return;
  }

  /* relative_index is in bounds, (forward) seek next partial key */
  for (int i = 0; i <= relative_index_; ++i) {
    cur_partial_key_ = node_->next_partial_key(cur_partial_key_);
  }
}

template <class T>
typename child_it<T>::reference child_it<T>::operator*() const {
  return cur_partial_key_;
}

template <class T>
typename child_it<T>::pointer child_it<T>::operator->() const {
  return &cur_partial_key_;
}

template <class T> child_it<T> &child_it<T>::operator++() {
  ++relative_index_;
  if (relative_index_ == 0) {
    cur_partial_key_ = node_->next_partial_key(0);
  } else if (relative_index_ < node_->n_children()) {
    cur_partial_key_ = node_->next_partial_key(cur_partial_key_ + 1);
  }
  return *this;
}

template <class T> child_it<T> child_it<T>::operator++(int) {
  auto old = *this;
  operator++();
  return old;
}

template <class T> child_it<T> &child_it<T>::operator--() {
  --relative_index_;
  if (relative_index_ == node_->n_children() - 1) {
    cur_partial_key_ = node_->prev_partial_key(255);
  } else if (relative_index_ >= 0) {
    cur_partial_key_ = node_->prev_partial_key(cur_partial_key_ - 1);
  }
  return *this;
}

template <class T> child_it<T> child_it<T>::operator--(int) {
  auto old = *this;
  operator--();
  return old;
}

template <class T> bool child_it<T>::operator==(const child_it<T> &rhs) const {
  /* should be from same node */
  assert(node_ == rhs.node_);
  return relative_index_ == rhs.relative_index_;
}

template <class T> bool child_it<T>::operator<(const child_it<T> &rhs) const {
  /* should be from same node */
  assert(node_ == rhs.node_);
  return relative_index_ < rhs.relative_index_;
}

template <class T> bool child_it<T>::operator!=(const child_it<T> &rhs) const {
  return !((*this) == rhs);
}

template <class T> bool child_it<T>::operator>=(const child_it<T> &rhs) const {
  return !((*this) < rhs);
}

template <class T> bool child_it<T>::operator<=(const child_it<T> &rhs) const {
  return (rhs >= (*this));
}

template <class T> bool child_it<T>::operator>(const child_it<T> &rhs) const {
  return (rhs < (*this));
}

} // namespace art

#endif
