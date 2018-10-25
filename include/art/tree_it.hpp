/**
 * @file tree iterator
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_TREE_IT_HPP
#define ART_TREE_IT_HPP

#include <cassert>
#include <iostream>
#include <iterator>
#include <stack>

namespace art {

template <class T> class node;

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
  /* preorder-traverse until node with value found or no nodes are left */
  while (!traversal_stack_.empty() &&
         traversal_stack_.top()->value_ == nullptr) {
    auto par = traversal_stack_.top();
    traversal_stack_.pop();
    for (auto it = par->rbegin(), it_end = par->rend(); it != it_end; ++it) {
      char child_partial_key = *it;
      traversal_stack_.push(*par->find_child(child_partial_key));
    }
  }
}

template <class T> 
tree_it<T> tree_it<T>::min(node<T> *root) {
  return tree_it<T>::greater_equal(root, "");
}

template <class T>
tree_it<T> tree_it<T>::greater_equal(node<T> *root, const char *key) {
  int depth, key_len = std::strlen(key) + 1;
  if (root == nullptr) {
    return tree_it<T>();
  }

  std::stack<node<T> *> node_stack;
  std::stack<int> depth_stack;
  node_stack.push(root);
  depth_stack.push(0);

  node<T> *cur;
  while (true) {
    cur = node_stack.top();
    depth = depth_stack.top();

    for (int i = 0; i < cur->prefix_len_; ++i) {
      if (depth + i + 1 == key_len) {
        goto exit_outer_loop;
      }
      if (cur->prefix_[i] < key[depth + i]) {
        node_stack.pop();
        goto exit_outer_loop;
      }
    }
    node_stack.pop();
    depth_stack.pop();
    auto child_it = cur->rbegin();
    auto child_it_end = cur->rend();
    for (; child_it != child_it_end; ++child_it) {
      auto partial_key = *child_it;
      if (partial_key < key[depth + cur->prefix_len_]) {
        break;
      }
      node_stack.push(*cur->find_child(partial_key));
      depth_stack.push(depth + cur->prefix_len_ + 1);
    }
  }
  exit_outer_loop:

  auto it = tree_it<T>(node_stack);
  if (*it == nullptr) {
    ++it;
  }
  return it;
}

template <class T> typename tree_it<T>::value_type tree_it<T>::operator*() {
  return traversal_stack_.top()->value_;
}

template <class T> typename tree_it<T>::pointer tree_it<T>::operator->() {
  return &traversal_stack_.top()->value_;
}

template <class T> tree_it<T> &tree_it<T>::operator++() {
  while (true) {
    node<T> *prev = traversal_stack_.top();
    traversal_stack_.pop();
    for (auto it = prev->rbegin(), it_end = prev->rend(); it != it_end; ++it) {
      char child_partial_key = *it;
      traversal_stack_.push(*prev->find_child(child_partial_key));
    }
    if (traversal_stack_.empty() || (traversal_stack_.top() != nullptr && traversal_stack_.top()->value_ != nullptr)) {
      break;
    }
  }
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
