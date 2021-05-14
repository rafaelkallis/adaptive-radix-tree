/**
 * @file child iterator
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_CHILD_IT_HPP
#define ART_CHILD_IT_HPP

#include <iterator>

namespace art {

template <class T> class inner_node;

template <class T> class child_it {
public:
  child_it() = default;
  explicit child_it(inner_node<T> *n);
  child_it(inner_node<T> *n, int relative_index);
  child_it(const child_it<T> &other) = default;
  child_it(child_it<T> &&other) noexcept = default;
  child_it<T> &operator=(const child_it<T> &other) = default;
  child_it<T> &operator=(child_it<T> &&other) noexcept = default;


  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = const char;
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

  char get_partial_key() const;
  node<T> *get_child_node() const;

private:
  inner_node<T> *node_ = nullptr;
  char cur_partial_key_ = -128;
  int relative_index_ = 0;
};

template <class T> child_it<T>::child_it(inner_node<T> *n) : child_it<T>(n, 0) {}

template <class T>
child_it<T>::child_it(inner_node<T> *n, int relative_index)
    : node_(n), cur_partial_key_(0), relative_index_(relative_index) {
  if (relative_index_ < 0) {
    /* relative_index is out of bounds, no seek */
    return;
  }

  if (relative_index_ >= node_->n_children()) {
    /* relative_index is out of bounds, no seek */
    return;
  }

  if (relative_index_ == node_->n_children() - 1) {
    cur_partial_key_ = node_->prev_partial_key(127);
    return;
  }

  cur_partial_key_ = node_->next_partial_key(-128);
  for (int i = 0; i < relative_index_; ++i) {
    cur_partial_key_ = node_->next_partial_key(cur_partial_key_ + 1);
  }
}

template <class T>
typename child_it<T>::reference child_it<T>::operator*() const {
  if (relative_index_ < 0 || relative_index_ >= node_->n_children()) {
    throw std::out_of_range("child iterator is out of range");
  }

  return cur_partial_key_;
}

template <class T>
typename child_it<T>::pointer child_it<T>::operator->() const {
  if (relative_index_ < 0 || relative_index_ >= node_->n_children()) {
    throw std::out_of_range("child iterator is out of range");
  }

  return &cur_partial_key_;
}

template <class T> child_it<T> &child_it<T>::operator++() {
  ++relative_index_;
  if (relative_index_ < 0) {
    return *this;
  } else if (relative_index_ == 0) {
    cur_partial_key_ = node_->next_partial_key(-128);
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
  if (relative_index_ > node_->n_children() - 1) {
    return *this;
  } else if (relative_index_ == node_->n_children() - 1) {
    cur_partial_key_ = node_->prev_partial_key(127);
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
  return node_ == rhs.node_ && relative_index_ == rhs.relative_index_;
}

template <class T> bool child_it<T>::operator<(const child_it<T> &rhs) const {
  return node_ == rhs.node_ && relative_index_ < rhs.relative_index_;
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

template <class T>
char child_it<T>::get_partial_key() const {
  return cur_partial_key_;
}

template <class T>
node<T> *child_it<T>::get_child_node() const {
  assert(0 <= relative_index_ && relative_index_ < node_->n_children());
  return *node_->find_child(cur_partial_key_);
}

} // namespace art

#endif
