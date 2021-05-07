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
#include <memory>
#include <string>

namespace art {

template <class T> class node;
template <class T> class inner_node;
template <class T> class leaf_node;

template <class T> class tree_it {
public:
  tree_it() = default;
  explicit tree_it(std::stack<node<T> *> traversal_stack, std::stack<const char *> key_stack, std::stack<int> depth_stack);
  ~tree_it();

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

  template <class OutputIterator> void key(OutputIterator key) const;
  const std::string key() const;

  int depth() const;

private:
  std::stack<node<T> *> traversal_stack_;
  /* important: also delete [] key on pop */
  std::stack<const char *> key_stack_;
  std::stack<int> depth_stack_;
};

template <class T>
tree_it<T>::tree_it(std::stack<node<T> *> traversal_stack, std::stack<const char *> key_stack, std::stack<int> depth_stack)
    : traversal_stack_(traversal_stack), key_stack_(key_stack), depth_stack_(depth_stack) {
  assert((traversal_stack.size() == key_stack.size()));
  assert((traversal_stack.size() == depth_stack.size()));

  inner_node<T> *cur_node;
  const char* cur_key;
  char* child_key;
  int cur_depth;
  char child_partial_key;
  node<T> *child;
  std::reverse_iterator<child_it<T>> child_it, child_it_end;

  /* seek: preorder-traverse until leaf node found or no nodes are left */
  while (!traversal_stack_.empty() && !traversal_stack_.top()->is_leaf()) {
    cur_node = static_cast<inner_node<T> *>(traversal_stack_.top());
    cur_key = key_stack_.top();
    cur_depth = depth_stack_.top();
    traversal_stack_.pop();
    key_stack_.pop();
    depth_stack_.pop();
    child_it = cur_node->rbegin();
    child_it_end = cur_node->rend();
    for (; child_it != child_it_end; ++child_it) {
      child_partial_key = *child_it;
      child = *cur_node->find_child(child_partial_key);

      /* compute child key: cur_key + cur_node->prefix_ + child_partial_key */
      child_key = new char[cur_depth + cur_node->prefix_len_ + 1];
      std::copy(cur_key, cur_key + cur_depth, child_key);
      std::copy(cur_node->prefix_, cur_node->prefix_ + cur_node->prefix_len_, child_key + cur_depth);
      child_key[cur_depth + cur_node->prefix_len_] = child_partial_key;

      traversal_stack_.push(child);
      key_stack_.push(child_key);
      depth_stack_.push(cur_depth + cur_node->prefix_len_ + 1);
    }
    delete [] cur_key;
  }
  
  assert((traversal_stack.size() == key_stack.size()));
  assert((traversal_stack.size() == depth_stack.size()));
}

template <class T> tree_it<T>::~tree_it() {
  while (!key_stack_.empty()) {
    delete [] key_stack_.top();
    key_stack_.pop();
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

  node<T> *cur_node;
  const char* cur_key;
  int cur_depth, key_len = std::strlen(key), i;
  std::reverse_iterator<child_it<T>> child_it, child_it_end;
  char partial_key, *child_key;
  std::stack<node<T> *> node_stack;
  std::stack<const char*> key_stack;
  std::stack<int> depth_stack;

  node_stack.push(root);
  key_stack.push(nullptr);
  depth_stack.push(0);

  while (true) {
    cur_node = node_stack.top();
    cur_key = key_stack.top();
    cur_depth = depth_stack.top();

    if (cur_depth == key_len) {
        return tree_it<T>(node_stack, key_stack, depth_stack);
    }
    for (i = 0; i < cur_node->prefix_len_; ++i) {
      if (cur_depth + i == key_len) {
        return tree_it<T>(node_stack, key_stack, depth_stack);
      }
      if (cur_node->prefix_[i] < key[cur_depth + i]) {
        node_stack.pop();
        key_stack.pop();
        depth_stack.pop();
        delete [] cur_key;
        return tree_it<T>(node_stack, key_stack, depth_stack);
      }
    }
    node_stack.pop();
    key_stack.pop();
    depth_stack.pop();
    if (!cur_node->is_leaf()) {
      // TODO: the following part is 1:1 with seek in constructor, is it possible to de-duplicate?
      child_it = static_cast<inner_node<T> *>(cur_node)->rbegin();
      child_it_end = static_cast<inner_node<T> *>(cur_node)->rend();
      for (; child_it != child_it_end; ++child_it)
      {
        partial_key = *child_it;
        if (partial_key < key[cur_depth + cur_node->prefix_len_])
        {
          break;
        }
        /* compute child key: cur_key + cur_node->prefix_ + child_partial_key */
        child_key = new char[cur_depth + cur_node->prefix_len_ + 1];
        std::copy(cur_key, cur_key + cur_depth, child_key);
        std::copy(cur_node->prefix_, cur_node->prefix_ + cur_node->prefix_len_, child_key + cur_depth);
        child_key[cur_depth + cur_node->prefix_len_] = partial_key;

        node_stack.push(*static_cast<inner_node<T> *>(cur_node)->find_child(partial_key));
        key_stack.push(child_key);
        depth_stack.push(cur_depth + cur_node->prefix_len_ + 1);
      }
    }
    delete [] cur_key;
  }
}

template <class T> typename tree_it<T>::value_type tree_it<T>::operator*() {
  return static_cast<leaf_node<T> *>(traversal_stack_.top())->value_;
}

template <class T> typename tree_it<T>::pointer tree_it<T>::operator->() {
  return &static_cast<leaf_node<T> *>(traversal_stack_.top())->value_;
}

template <class T> tree_it<T> &tree_it<T>::operator++() {
  assert((traversal_stack_.size() == key_stack_.size()));
  assert((traversal_stack_.size() == depth_stack_.size()));

  inner_node<T> *cur_node;
  const char *cur_key;
  char *child_key;
  int cur_depth;
  std::reverse_iterator<child_it<T>> it, it_end;

  delete [] key_stack_.top();
  traversal_stack_.pop();
  key_stack_.pop();
  depth_stack_.pop();
  while (!traversal_stack_.empty() && !traversal_stack_.top()->is_leaf()) {
    cur_node = static_cast<inner_node<T> *>(traversal_stack_.top());
    cur_key = key_stack_.top();
    cur_depth = depth_stack_.top();
    traversal_stack_.pop();
    key_stack_.pop();
    depth_stack_.pop();
    for (it = cur_node->rbegin(), it_end = cur_node->rend(); it != it_end; ++it) {
      /* compute child key: cur_key + cur_node->prefix_ + child_partial_key */
      child_key = new char[cur_depth + cur_node->prefix_len_ + 1];
      std::copy(cur_key, cur_key + cur_depth, child_key);
      std::copy(cur_node->prefix_, cur_node->prefix_ + cur_node->prefix_len_, child_key + cur_depth);
      child_key[cur_depth + cur_node->prefix_len_] = *it;

      traversal_stack_.push(*cur_node->find_child(*it));
      key_stack_.push(child_key);
      depth_stack_.push(cur_depth + cur_node->prefix_len_ + 1);
    }
    delete [] cur_key;
  };
  
  assert((traversal_stack_.size() == depth_stack_.size()));
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

template <class T> 
template <class OutputIterator> 
void tree_it<T>::key(OutputIterator key) const {
  std::copy(key_stack_.top(), key_stack_.top() + depth_stack_.top(), key);
  std::copy(traversal_stack_.top()->prefix_, traversal_stack_.top()->prefix_ + traversal_stack_.top()->prefix_len_, key + depth_stack_.top());
}

template <class T>
const std::string tree_it<T>::key() const {
  std::string str(depth() - 1, 0);
  key(str.begin());
  return str;
}

template <class T> 
int tree_it<T>::depth() const {
  return depth_stack_.top() + traversal_stack_.top()->prefix_len_;
}

} // namespace art

#endif
