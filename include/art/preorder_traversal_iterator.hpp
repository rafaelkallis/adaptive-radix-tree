/**
 * @file preorder traversal iterator
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_PREORDER_TRAVERSAL_ITERATOR_HPP
#define ART_PREORDER_TRAVERSAL_ITERATOR_HPP

#include "constants.hpp"
#include <iterator>
#include <stack>

namespace art {

template <class T> class node;

template <class T> class preorder_traversal_iterator {
public:
  explicit preorder_traversal_iterator(node<T> *root);

  using iterator_category = std::forward_iterator_tag;
  using value_type = node<T> *;
  using difference_type = int;
  using pointer = value_type *;
  using reference = value_type &;

  reference operator*();
  pointer operator->();
  preorder_traversal_iterator<T> &operator++();
  preorder_traversal_iterator<T> operator++(int);
  bool operator==(const preorder_traversal_iterator<T> &rhs) const;
  bool operator!=(const preorder_traversal_iterator<T> &rhs) const;

private:
  std::stack<node<T> *> stack_;
};

template <class T>
preorder_traversal_iterator<T>::preorder_traversal_iterator(node<T> *root)
    : stack_() {
  if (root != nullptr) {
    stack_.push(root);
  }
}

template <class T>
typename preorder_traversal_iterator<T>::reference preorder_traversal_iterator<T>::
operator*() {
  return stack_.top();
}

template <class T>
typename preorder_traversal_iterator<T>::pointer preorder_traversal_iterator<T>::
operator->() {
  return &stack_.top();
}

template <class T>
preorder_traversal_iterator<T> &preorder_traversal_iterator<T>::operator++() {
  node<T> *prev = stack_.top();
  stack_.pop();
  for (auto it = prev->rbegin(), it_end = prev->rend(); it != it_end; ++it) {
    partial_key_type child_partial_key = *it;
    stack_.push(*prev->find_child(child_partial_key));
  }
  return *this;
}

template <class T>
preorder_traversal_iterator<T> preorder_traversal_iterator<T>::operator++(int) {
  auto old = *this;
  operator++();
  return old;
}

template <class T>
bool preorder_traversal_iterator<T>::
operator==(const preorder_traversal_iterator<T> &rhs) const {
  return (stack_.empty() && rhs.stack_.empty()) ||
         (!stack_.empty() && !rhs.stack_.empty() &&
          stack_.top() == rhs.stack_.top());
}

template <class T>
bool preorder_traversal_iterator<T>::
operator!=(const preorder_traversal_iterator<T> &rhs) const {
  return !(*this == rhs);
}

} // namespace art

#endif
