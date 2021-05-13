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
#include <deque>

namespace art {

template <class T> class node;
template <class T> class inner_node;
template <class T> class leaf_node;

template <class T> class tree_it {
public:
  struct step {
    node<T> *node_;
    int depth_;
    child_it<T> child_it_;
    child_it<T> child_it_end_;

    step(int depth, child_it<T> c_it, child_it<T> c_it_end);
    step(node<T> *node, int depth, child_it<T> c_it, child_it<T> c_it_end);

    step &operator++();
    step operator++(int);
  };

  tree_it() = default;
  explicit tree_it(std::deque<step> traversal_stack);

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

  node<T> *get_node() const;
  int get_depth() const;

private:
  step &get_step();
  const step &get_step() const;
  void seek_leaf();

  std::deque<step> traversal_stack_;
};

template <class T>
tree_it<T>::step::step(node<T> *node, int depth, child_it<T> c_it, child_it<T> c_it_end) 
  : node_(node), depth_(depth), child_it_(c_it), child_it_end_(c_it_end) {}

template <class T>
tree_it<T>::step::step(int depth, child_it<T> c_it, child_it<T> c_it_end) 
  : step(c_it != c_it_end ? c_it.get_child_node() : nullptr, depth, c_it, c_it_end) {}

template <class T> 
typename tree_it<T>::step &tree_it<T>::step::operator++() {
  assert(child_it_ != child_it_end_);
  ++child_it_;
  node_ = child_it_ != child_it_end_ 
    ? child_it_.get_child_node()
    : nullptr;
  return *this;
}

template <class T> 
typename tree_it<T>::step tree_it<T>::step::operator++(int) {
  auto old = *this;
  operator++();
  return old;
}

template <class T>
tree_it<T>::tree_it(std::deque<step> traversal_stack) : traversal_stack_(traversal_stack) {
  seek_leaf();
}

template <class T> tree_it<T> tree_it<T>::min(node<T> *root) {
  return tree_it<T>::greater_equal(root, "");
}

template <class T>
tree_it<T> tree_it<T>::greater_equal(node<T> *root, const char *key) {
  assert(root != nullptr);

  int key_len = std::strlen(key);
  inner_node<T> *cur_inner_node;
  child_it<T> child_it, child_it_end;
  std::deque<tree_it<T>::step> traversal_stack;

  // sentinel child iterator for root
  traversal_stack.push_back({root, 0, {nullptr, -2}, {nullptr, -1}});

  while (true) {
    tree_it<T>::step &cur_step = traversal_stack.back();
    node<T> *cur_node = cur_step.node_;
    int cur_depth = cur_step.depth_;

    int prefix_match_len = std::min<int>(cur_node->check_prefix(key + cur_depth, key_len - cur_depth), key_len - cur_depth);
    // if search key "equals" the prefix
    if (key_len == cur_depth + prefix_match_len) {
        return tree_it<T>(traversal_stack);
    }
    // if search key is "greater than" the prefix
    if (prefix_match_len < cur_node->prefix_len_ &&  key[cur_depth + prefix_match_len] > cur_node->prefix_[prefix_match_len]) {
      ++cur_step;
      return tree_it<T>(traversal_stack);
    }
    if (cur_node->is_leaf()) {
      continue;
    }
    // seek subtree where search key is "lesser than or equal" the subtree partial key
    cur_inner_node = static_cast<inner_node<T> *>(cur_node);
    child_it = cur_inner_node->begin();
    child_it_end = cur_inner_node->end();
    // TODO more efficient with specialized node search method?
    for (; child_it != child_it_end; ++child_it) {
      if (key[cur_depth + cur_node->prefix_len_] <= child_it.get_partial_key()) {
        break;
      }
    }
    traversal_stack.push_back({cur_depth + cur_node->prefix_len_ + 1, child_it, child_it_end});
  }
}

template <class T> typename tree_it<T>::value_type tree_it<T>::operator*() {
  assert(get_node()->is_leaf());
  return static_cast<leaf_node<T> *>(get_node())->value_;
}

template <class T> typename tree_it<T>::pointer tree_it<T>::operator->() {
  assert(get_node()->is_leaf());
  return &static_cast<leaf_node<T> *>(get_node())->value_;
}

template <class T> tree_it<T> &tree_it<T>::operator++() {
  assert(get_node()->is_leaf());
  ++get_step();
  seek_leaf();
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
          get_node() == rhs.get_node());
}

template <class T> bool tree_it<T>::operator!=(const tree_it<T> &rhs) const {
  return !(*this == rhs);
}

template <class T>
node<T> * tree_it<T>::get_node() const {
  return get_step().node_;
}

template <class T>
int tree_it<T>::get_depth() const {
  return get_step().depth_;
}

template <class T> 
typename tree_it<T>::step &tree_it<T>::get_step() {
  assert(!traversal_stack_.empty());
  return traversal_stack_.back();
}

template <class T> 
const typename tree_it<T>::step &tree_it<T>::get_step() const {
  assert(!traversal_stack_.empty());
  return traversal_stack_.back();
}

template <class T>
void tree_it<T>::seek_leaf() {

  /* traverse up until a node on the right is found or stack gets empty */
  for (; get_step().child_it_ == get_step().child_it_end_; ++get_step()) {
    traversal_stack_.pop_back();
    if (traversal_stack_.empty()) {
      return;
    }
  }
  
  /* find leftmost leaf node */
  while (!get_node()->is_leaf()) {
    inner_node<T> *cur_inner_node = static_cast<inner_node<T> *>(get_node());
    int depth = get_step().depth_ + get_node()->prefix_len_ + 1;
    child_it<T> c_it = cur_inner_node->begin();
    child_it<T> c_it_end = cur_inner_node->end();
    traversal_stack_.push_back({depth, c_it, c_it_end});
  }
}

} // namespace art

#endif
