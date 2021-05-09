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
  struct step {
    node<T> *node_;
    int depth_;
    /* important: also delete [] key on pop */
    const char *key_;
  };

  tree_it() = default;
  explicit tree_it(std::stack<step *> traversal_stack);
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
  void seek_leaf();
  std::stack<step *> traversal_stack_;
};

template <class T>
tree_it<T>::tree_it(std::stack<tree_it<T>::step *> traversal_stack) : traversal_stack_(traversal_stack) {
  seek_leaf();
}

template <class T> 
tree_it<T>::~tree_it() {
  while (!traversal_stack_.empty()) {
    delete [] traversal_stack_.top()->key_;
    delete traversal_stack_.top();
    traversal_stack_.pop();
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

  tree_it<T>::step *cur;
  int key_len = std::strlen(key), child_depth, i;
  node<T> *child_node;
  inner_node<T> *cur_in_node;
  std::reverse_iterator<child_it<T>> it, it_end;
  char *child_key;
  std::stack<tree_it<T>::step *> traversal_stack;

  traversal_stack.push(new tree_it<T>::step {root, 0, nullptr});

  while (true) {
    cur = traversal_stack.top();
    if (cur->depth_ == key_len) {
        return tree_it<T>(traversal_stack);
    }
    for (i = 0; i < cur->node_->prefix_len_; ++i) {
      if (cur->depth_ + i == key_len) {
        return tree_it<T>(traversal_stack);
      }
      if (cur->node_->prefix_[i] < key[cur->depth_ + i]) {
        traversal_stack.pop();
        delete [] cur->key_;
        delete cur;
        return tree_it<T>(traversal_stack);
      }
    }
    traversal_stack.pop();
    if (!cur->node_->is_leaf()) {
      // TODO: the following part is 1:1 with seek in constructor, is it possible to de-duplicate?
      cur_in_node = static_cast<inner_node<T> *>(cur->node_);
      for (it = cur_in_node->rbegin(), it_end = cur_in_node->rend(); it != it_end; ++it) {
        if (*it < key[cur->depth_ + cur->node_->prefix_len_]) {
          break;
        }
        child_node = *cur_in_node->find_child(*it);
        child_depth = cur->depth_ + cur->node_->prefix_len_ + 1;
        /* compute child key: cur_key + cur_node->prefix_ + child_partial_key */
        child_key = new char[cur->depth_ + cur->node_->prefix_len_ + 1];
        std::copy(cur->key_, cur->key_ + cur->depth_, child_key);
        std::copy(cur->node_->prefix_, cur->node_->prefix_ + cur->node_->prefix_len_, child_key + cur->depth_);
        child_key[cur->depth_ + cur->node_->prefix_len_] = *it;

        traversal_stack.push(new tree_it<T>::step {child_node, child_depth, child_key});
      }
    }
    delete [] cur->key_;
    delete cur;
  }
}

template <class T> typename tree_it<T>::value_type tree_it<T>::operator*() {
  assert((traversal_stack_.top()->node_->is_leaf()));
  return static_cast<leaf_node<T> *>(traversal_stack_.top()->node_)->value_;
}

template <class T> typename tree_it<T>::pointer tree_it<T>::operator->() {
  assert((traversal_stack_.top()->node_->is_leaf()));
  return &static_cast<leaf_node<T> *>(traversal_stack_.top()->node_)->value_;
}

template <class T> tree_it<T> &tree_it<T>::operator++() {
  assert(!traversal_stack_.empty());
  delete [] traversal_stack_.top()->key_;
  delete traversal_stack_.top();
  traversal_stack_.pop();
  seek_leaf();
  return *this;
}

template <class T> tree_it<T> tree_it<T>::operator++(int) {
  auto old = *this;
  operator++();
  return old;
}

template <class T> bool tree_it<T>::operator==(const tree_it<T> &rhs) const {
  if (traversal_stack_.empty() && rhs.traversal_stack_.empty()) {
    /* both are empty */
    return true;
  }
  if (traversal_stack_.empty() || rhs.traversal_stack_.empty()) {
    /* one is empty */
    return false;
  }
  return traversal_stack_.top()->node_ == rhs.traversal_stack_.top()->node_;
}

template <class T> bool tree_it<T>::operator!=(const tree_it<T> &rhs) const {
  return !(*this == rhs);
}

template <class T> 
template <class OutputIterator> 
void tree_it<T>::key(OutputIterator key) const {
  tree_it<T>::step * cur = traversal_stack_.top();
  std::copy(cur->key_, cur->key_ + cur->depth_, key);
  std::copy(cur->node_->prefix_, cur->node_->prefix_ + cur->node_->prefix_len_, key + cur->depth_);
}

template <class T>
const std::string tree_it<T>::key() const {
  std::string str(depth() - 1, 0);
  key(str.begin());
  return str;
}

template <class T> 
int tree_it<T>::depth() const {
  tree_it<T>::step * cur = traversal_stack_.top();
  return cur->depth_ + cur->node_->prefix_len_;
}

template <class T> 
void tree_it<T>::seek_leaf() {
  tree_it<T>::step * cur;
  inner_node<T> *cur_in_node;
  node<T> *child_node;
  int child_depth;
  char* child_key;
  std::reverse_iterator<child_it<T>> child_it, child_it_end;

  /* preorder-traverse until leaf node found or no nodes are left */
  while (!traversal_stack_.empty() && !traversal_stack_.top()->node_->is_leaf()) {
    cur = traversal_stack_.top();
    traversal_stack_.pop();
    cur_in_node = static_cast<inner_node<T> *>(cur->node_);
    for (child_it = cur_in_node->rbegin(), child_it_end = cur_in_node->rend(); child_it != child_it_end; ++child_it) {
      child_node = *cur_in_node->find_child(*child_it);
      child_depth = cur->depth_ + cur_in_node->prefix_len_ + 1;

      /* compute child key: cur_key + cur_node->prefix_ + child_partial_key */
      child_key = new char[cur->depth_ + cur_in_node->prefix_len_ + 1];
      std::copy(cur->key_, cur->key_ + cur->depth_, child_key);
      std::copy(cur_in_node->prefix_, cur_in_node->prefix_ + cur_in_node->prefix_len_, child_key + cur->depth_);
      child_key[cur->depth_ + cur_in_node->prefix_len_] = *child_it;

      traversal_stack_.push(new tree_it<T>::step {child_node, child_depth, child_key});
    }
    delete [] cur->key_;
    delete cur;
  }
}

} // namespace art

#endif
