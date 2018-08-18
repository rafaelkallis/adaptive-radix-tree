/**
 * @file preorder traversal iterator
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_PREORDER_TRAVERSAL_ITERATOR_HPP
#define ART_PREORDER_TRAVERSAL_ITERATOR_HPP

#include "constants.hpp"
#include <cassert>
#include <iterator>
#include <stack>
#include <string>

namespace art {

using std::string;

template <class T> class node;

template <class T> class preorder_traversal_iterator {
public:
  preorder_traversal_iterator() = default;
  explicit preorder_traversal_iterator(node<T> *root);
  explicit preorder_traversal_iterator(std::stack<node<T> *> traversal_stack);

  static preorder_traversal_iterator<T> from_min_key(node<T> *root,
                                                     key_type key, int key_len);

  using iterator_category = std::forward_iterator_tag;
  using value_type = T *;
  using difference_type = int;
  using pointer = value_type *;
  /* using reference = const value_type &; */

  /* reference operator*(); */
  value_type operator*();
  pointer operator->();
  preorder_traversal_iterator<T> &operator++();
  preorder_traversal_iterator<T> operator++(int);
  bool operator==(const preorder_traversal_iterator<T> &rhs) const;
  bool operator!=(const preorder_traversal_iterator<T> &rhs) const;

private:
  std::stack<node<T> *> traversal_stack_;
};

template <class T>
preorder_traversal_iterator<T>::preorder_traversal_iterator(node<T> *root)
    : traversal_stack_() {
  assert(root != nullptr);
  traversal_stack_.push(root);
}

template <class T>
preorder_traversal_iterator<T>::preorder_traversal_iterator(
    std::stack<node<T> *> traversal_stack)
    : traversal_stack_(traversal_stack) {
  /* preorder-traverse until node with value found or no nodes are left */
  while (!traversal_stack_.empty() &&
         traversal_stack_.top()->get_value() == nullptr) {
    node<T> *prev = traversal_stack_.top();
    traversal_stack_.pop();
    for (auto it = prev->rbegin(), it_end = prev->rend(); it != it_end; ++it) {
      partial_key_type child_partial_key = *it;
      traversal_stack_.push(*prev->find_child(child_partial_key));
    }
  }
}

template <class T>
preorder_traversal_iterator<T>
preorder_traversal_iterator<T>::from_min_key(node<T> *root, key_type key,
                                             int key_len) {
  assert(root != nullptr);
  std::stack<node<T> *> node_stack;
  std::stack<int> depth_stack;
  node_stack.push(root);
  depth_stack.push(0);
  while (true) {
    auto cur_node = node_stack.top();
    auto depth = depth_stack.top();
    auto prefix = cur_node->get_prefix();
    auto prefix_len = prefix.size();

    for (int i = 0; i < prefix_len; ++i) {
      if (depth + i + 1 == key_len) {
        return preorder_traversal_iterator<T>(node_stack);
      }
      if (prefix[i] < key[depth + i]) {
        node_stack.pop();
        return preorder_traversal_iterator<T>(node_stack);
      }
    }
    node_stack.pop();
    depth_stack.pop();
    for (auto child_it = cur_node->rbegin(), child_it_end = cur_node->rend();
         child_it != child_it_end; ++child_it) {
      auto partial_key = *child_it;
      if (partial_key < key[depth + prefix_len]) {
        break;
      }
      node_stack.push(*cur_node->find_child(partial_key));
      depth_stack.push(depth + prefix_len + 1);
    }
  }

  return preorder_traversal_iterator<T>(node_stack);
}

template <class T>
typename preorder_traversal_iterator<T>::value_type
    preorder_traversal_iterator<T>::operator*() {
  return traversal_stack_.top()->get_value();
}

template <class T>
typename preorder_traversal_iterator<T>::pointer
    preorder_traversal_iterator<T>::operator->() {
  return &traversal_stack_.top()->get_value();
}

template <class T>
preorder_traversal_iterator<T> &preorder_traversal_iterator<T>::operator++() {
  while (true) {
    node<T> *prev = traversal_stack_.top();
    traversal_stack_.pop();
    for (auto it = prev->rbegin(), it_end = prev->rend(); it != it_end; ++it) {
      partial_key_type child_partial_key = *it;
      traversal_stack_.push(*prev->find_child(child_partial_key));
    }
    if (traversal_stack_.empty() || traversal_stack_.top() != nullptr) {
      break;
    }
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
  return (traversal_stack_.empty() && rhs.traversal_stack_.empty()) ||
         (!traversal_stack_.empty() && !rhs.traversal_stack_.empty() &&
          traversal_stack_.top() == rhs.traversal_stack_.top());
}

template <class T>
bool preorder_traversal_iterator<T>::
operator!=(const preorder_traversal_iterator<T> &rhs) const {
  return !(*this == rhs);
}

} // namespace art

#endif
