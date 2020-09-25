/**
 * @file tree iterator
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_TREE_IT_HPP
#define ART_TREE_IT_HPP

#include "child_it.hpp"
#include <cassert>
#include <cstring>
#include <iostream>
#include <iterator>
#include <stack>

namespace art {

template <class T> class node;
template <class T> class inner_node;
template <class T> class leaf_node;

template <class T> class tree_it {
public:
  tree_it() = default;
  explicit tree_it(std::stack<node<T> *> traversal_stack);

  static tree_it<T> min(node<T> *root);
  static tree_it<T> greater_equal(node<T> *root, const char *key);

  using iterator_category = std::forward_iterator_tag;
  using value_type = T *;
  using difference_type = int;
  using pointer = value_type *;
  /* using reference = const value_type &; */

  /* reference operator*(); */
  value_type operator*();
  pointer operator->();
  tree_it<T> &operator++();
  tree_it<T> operator++(int);
  bool operator==(const tree_it<T> &rhs) const;
  bool operator!=(const tree_it<T> &rhs) const;

private:
  std::stack<node<T> *> traversal_stack_;
};

template <class T>
tree_it<T>::tree_it(std::stack<node<T> *> traversal_stack)
    : traversal_stack_(traversal_stack) {
  inner_node<T> *cur;
  char child_partial_key;
  std::reverse_iterator<child_it<T>> it, it_end;

  /* preorder-traverse until leaf node found or no nodes are left */
  while (!traversal_stack_.empty() && !traversal_stack_.top()->is_leaf()) {
    cur = static_cast<inner_node<T> *>(traversal_stack_.top());
    traversal_stack_.pop();
    for (it = cur->rbegin(), it_end = cur->rend(); it != it_end; ++it) {
      child_partial_key = *it;
      traversal_stack_.push(*cur->find_child(child_partial_key));
    }
  }
}

template <class T> tree_it<T> tree_it<T>::min(node<T> *root) {
  return tree_it<T>::greater_equal(root, "");
}

template <class T>
tree_it<T> tree_it<T>::greater_equal(node<T> *root, const char *key) {
  if (root == nullptr) {
    return tree_it<T>();
  }

  int cur_depth, key_len = std::strlen(key) + 1, i;
  node<T> *cur;
  std::reverse_iterator<child_it<T>> it, it_end;
  char partial_key;
  std::stack<node<T> *> node_stack;
  std::stack<int> depth_stack;

  node_stack.push(root);
  depth_stack.push(0);

  while (true) {
    cur = node_stack.top();
    cur_depth = depth_stack.top();

    for (i = 0; i < cur->prefix_len_; ++i) {
      if (cur_depth + i + 1 == key_len) {
        goto exit_outer_loop;
      }
      if (cur->prefix_[i] < key[cur_depth + i]) {
        node_stack.pop();
        goto exit_outer_loop;
      }
    }
    node_stack.pop();
    depth_stack.pop();
    if (cur->is_leaf()) {
      continue;
    }
    it = static_cast<inner_node<T> *>(cur)->rbegin();
    it_end = static_cast<inner_node<T> *>(cur)->rend();
    for (; it != it_end; ++it) {
      partial_key = *it;
      if (partial_key < key[cur_depth + cur->prefix_len_]) {
        break;
      }
      node_stack.push(
          *static_cast<inner_node<T> *>(cur)->find_child(partial_key));
      depth_stack.push(cur_depth + cur->prefix_len_ + 1);
    }
  }
exit_outer_loop:

  return tree_it<T>(node_stack);
}

template <class T> typename tree_it<T>::value_type tree_it<T>::operator*() {
  return static_cast<leaf_node<T> *>(traversal_stack_.top())->value_;
}

template <class T> typename tree_it<T>::pointer tree_it<T>::operator->() {
  return &static_cast<leaf_node<T> *>(traversal_stack_.top())->value_;
}

template <class T> tree_it<T> &tree_it<T>::operator++() {
  inner_node<T> *cur;
  std::reverse_iterator<child_it<T>> it, it_end;

  traversal_stack_.pop();
  while (!traversal_stack_.empty() && !traversal_stack_.top()->is_leaf()) {
    cur = static_cast<inner_node<T> *>(traversal_stack_.top());
    traversal_stack_.pop();
    for (it = cur->rbegin(), it_end = cur->rend(); it != it_end; ++it) {
      traversal_stack_.push(*cur->find_child(*it));
    }
  };
  return *this;
}

template <class T> tree_it<T> tree_it<T>::operator++(int) {
  auto old = *this;
  operator++();
  return old;
}

template <class T> bool tree_it<T>::operator==(const tree_it<T> &rhs) const {
  return (traversal_stack_.empty() && rhs.traversal_stack_.empty()) ||
         (!traversal_stack_.empty() && !rhs.traversal_stack_.empty() &&
          traversal_stack_.top() == rhs.traversal_stack_.top());
}

template <class T> bool tree_it<T>::operator!=(const tree_it<T> &rhs) const {
  return !(*this == rhs);
}

} // namespace art

#endif
