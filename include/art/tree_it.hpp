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
#include <utility>
#include <vector>

namespace art {

template <class T> class node;
template <class T> class inner_node;
template <class T> class leaf_node;

template <class T> class tree_it {
public:
  struct step {
    const node<T> *child_node_; // no ownership
    int depth_;
    char *key_; // ownership
    child_it<T> child_it_;
    child_it<T> child_it_end_;

    step();
    step(int depth, child_it<T> c_it, child_it<T> c_it_end);
    step(const node<T> *node, int depth, const char *key, child_it<T> c_it, child_it<T> c_it_end);
    step(const step &other);
    step(step &&other);
    ~step();
    
    step& operator=(const step &other);
    step& operator=(step &&other) noexcept;

    step &operator++();
    step operator++(int);
  };

  tree_it();
  explicit tree_it(const node<T> *root, std::vector<step> traversal_stack);

  static tree_it<T> min(const node<T> *root);
  static tree_it<T> greater_equal(const node<T> *root, const char *key);

  using iterator_category = std::forward_iterator_tag;
  using value_type = const T;
  using difference_type = int;
  using pointer = value_type *;
  using reference = value_type &;

  reference operator*() const;
  pointer operator->() const;
  tree_it<T> &operator++();
  tree_it<T> operator++(int);
  bool operator==(const tree_it<T> &rhs) const;
  bool operator!=(const tree_it<T> &rhs) const;

  template <class OutputIt> void key(OutputIt key) const;
  int get_key_len() const;
  const std::string key() const;
  

private:
  step &get_step();
  const step &get_step() const;
  const node<T> *get_child_node() const;
  const char *get_key() const;
  int get_depth() const;
  
  void seek_leaf();

  const node<T> *root_;
  std::vector<step> traversal_stack_;
};

template <class T>
tree_it<T>::step::step() 
  : step(nullptr, 0, nullptr, {}, {}) {}

template <class T>
tree_it<T>::step::step(int depth, child_it<T> c_it, child_it<T> c_it_end) 
  : child_node_(c_it != c_it_end ? c_it.get_child_node() : nullptr), 
    depth_(depth), 
    key_(depth ? new char[depth] : nullptr), 
    child_it_(c_it), 
    child_it_end_(c_it_end) {}

template <class T>
tree_it<T>::step::step(const node<T> *node, int depth, const char *key, child_it<T> c_it, child_it<T> c_it_end) 
  : child_node_(node), 
  depth_(depth), 
  key_(depth ? new char[depth] : nullptr), 
  child_it_(c_it), 
  child_it_end_(c_it_end) {
    std::copy_n(key, depth, key_);
  }

template <class T> 
typename tree_it<T>::step &tree_it<T>::step::operator++() {
  assert(child_it_ != child_it_end_);
  ++child_it_;
  child_node_ = child_it_ != child_it_end_ 
    ? child_it_.get_child_node()
    : nullptr;
  key_[depth_ - 1] = child_it_ != child_it_end_
    ? child_it_.get_partial_key()
    : '\0';
  return *this;
}

template <class T> 
typename tree_it<T>::step tree_it<T>::step::operator++(int) {
  auto old = *this;
  operator++();
  return old;
}

template <class T>
tree_it<T>::step::step(const tree_it<T>::step &other) 
  : step(other.child_node_, other.depth_, other.key_, other.child_it_, other.child_it_end_) {}

template <class T>
tree_it<T>::step::step(tree_it<T>::step &&other) 
  : child_node_(other.child_node_), 
    depth_(other.depth_), 
    key_(other.key_),
    child_it_(other.child_it_),
    child_it_end_(other.child_it_end_) {
   other.child_node_ = nullptr;
   other.depth_ = 0;
   other.key_ = nullptr;
   other.child_it_ = {};
   other.child_it_end_ = {};
}

template <class T>
tree_it<T>::step::~step() {
  delete [] key_;
}

template <class T>
typename tree_it<T>::step& tree_it<T>::step::operator=(const tree_it<T>::step &other) {
  if (this != &other) {
    node<T> *node = other.child_node_;
    int depth = other.depth_;
    char *key = depth ? new char[depth] : nullptr;
    std::copy_n(other.key_, other.depth_, key);
    child_it<T> c_it = other.child_it_;
    child_it<T> c_it_end = other.child_it_end_;

    child_node_ = node;
    depth_ = depth;
    delete [] key_;
    key_ = key;
    child_it_ = c_it;
    child_it_end_ = c_it_end;
  }
  return *this;
}

template <class T>
typename tree_it<T>::step& tree_it<T>::step::operator=(tree_it<T>::step &&other) noexcept {
  if (this != &other) {
    child_node_ = other.child_node_;
    other.child_node_ = nullptr;

    depth_ = other.depth_;
    other.depth_ = 0;

    delete [] key_;
    key_ = other.key_;
    other.key_ = nullptr;

    child_it_ = other.child_it_;
    other.child_it_ = {};

    child_it_end_ = other.child_it_end_;
    other.child_it_end_ = {};
  }
  return *this;
}

template <class T>
tree_it<T>::tree_it() {}

template <class T>
tree_it<T>::tree_it(const node<T> *root, std::vector<step> traversal_stack) : root_(root), traversal_stack_(traversal_stack) {
  seek_leaf();
}

template <class T> tree_it<T> tree_it<T>::min(const node<T> *root) {
  return tree_it<T>::greater_equal(root, "");
}

template <class T>
tree_it<T> tree_it<T>::greater_equal(const node<T> *root, const char *key) {
  assert(root != nullptr);

  int key_len = std::strlen(key);
  std::vector<tree_it<T>::step> traversal_stack;

  // sentinel child iterator for root
  traversal_stack.push_back({root, 0, nullptr, {nullptr, -2}, {nullptr, -1}});

  while (true) {
    tree_it<T>::step &cur_step = traversal_stack.back();
    const node<T> *cur_node = cur_step.child_node_;
    int cur_depth = cur_step.depth_;

    int prefix_match_len = std::min<int>(cur_node->check_prefix(key + cur_depth, key_len - cur_depth), key_len - cur_depth);
    // if search key "equals" the prefix
    if (key_len == cur_depth + prefix_match_len) {
        return tree_it<T>(root, traversal_stack);
    }
    // if search key is "greater than" the prefix
    if (prefix_match_len < cur_node->prefix_len_ &&  key[cur_depth + prefix_match_len] > cur_node->prefix_[prefix_match_len]) {
      ++cur_step;
      return tree_it<T>(root, traversal_stack);
    }
    if (cur_node->is_leaf()) {
      continue;
    }
    // seek subtree where search key is "lesser than or equal" the subtree partial key
    child_it<T> c_it = cur_node->as_inner()->begin();
    child_it<T> c_it_end = cur_node->as_inner()->end();
    // TODO more efficient with specialized node search method?
    for (; c_it != c_it_end; ++c_it) {
      if (key[cur_depth + cur_node->prefix_len_] <= c_it.get_partial_key()) {
        break;
      }
    }
    int depth = cur_depth + cur_node->prefix_len_ + 1;
    tree_it<T>::step child(depth, c_it, c_it_end);
    /* compute child key: cur_key + cur_node->prefix_ + child_partial_key */
    std::copy_n(cur_step.key_, cur_depth, child.key_);
    std::copy_n(cur_node->prefix_, cur_node->prefix_len_, child.key_ + cur_depth);
    child.key_[cur_depth + cur_node->prefix_len_] = c_it.get_partial_key();
    traversal_stack.push_back(child);
  }
}

template <class T> typename tree_it<T>::reference tree_it<T>::operator*() const {
  return get_child_node()->as_leaf()->value_;
}

template <class T> typename tree_it<T>::pointer tree_it<T>::operator->() const {
  return &get_child_node()->as_leaf()->value_;
}

template <class T> tree_it<T> &tree_it<T>::operator++() {
  assert(get_child_node()->is_leaf());
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
  if (traversal_stack_.empty() && rhs.traversal_stack_.empty()) {
    /* both are empty */
    return true;
  }
  if (traversal_stack_.empty() || rhs.traversal_stack_.empty()) {
    /* one is empty */
    return false;
  }
  return get_child_node() == rhs.get_child_node();
}

template <class T> bool tree_it<T>::operator!=(const tree_it<T> &rhs) const {
  return !(*this == rhs);
}

template <class T> 
template <class OutputIt> 
void tree_it<T>::key(OutputIt key) const {
  std::copy_n(get_key(), get_depth(), key);
  std::copy_n(get_child_node()->prefix_, get_child_node()->prefix_len_, key + get_depth());
}

template <class T>
int tree_it<T>::get_key_len() const {
  return get_depth() + get_child_node()->prefix_len_;
}

template <class T>
const std::string tree_it<T>::key() const {
  std::string str(get_depth() + get_child_node()->prefix_len_ - 1, 0);
  key(str.begin());
  return str;
}

template <class T>
void tree_it<T>::seek_leaf() {
  /* traverse up until a node on the right is found or stack gets empty */
  for (; get_step().child_it_ == get_step().child_it_end_; ++get_step()) {
    traversal_stack_.pop_back();
    if (get_step().child_node_ == root_) { // root guard
      traversal_stack_.pop_back();
      assert(traversal_stack_.empty());
      return;
    }
  }
  
  /* find leftmost leaf node */
  while (!get_child_node()->is_leaf()) {
    int depth = get_depth() + get_child_node()->prefix_len_ + 1;
    child_it<T> c_it = get_child_node()->as_inner()->begin();
    child_it<T> c_it_end = get_child_node()->as_inner()->end();
    tree_it<T>::step child(depth, c_it, c_it_end);
    /* compute child key: cur_key + cur_node->prefix_ + child_partial_key */
    std::copy_n(get_key(), get_depth(), child.key_);
    std::copy_n(get_child_node()->prefix_, get_child_node()->prefix_len_, child.key_ + get_depth());
    child.key_[get_depth() + get_child_node()->prefix_len_] = c_it.get_partial_key();
    traversal_stack_.push_back(child);
  }
}

template <class T>
const node<T> * tree_it<T>::get_child_node() const {
  return get_step().child_node_;
}

template <class T>
int tree_it<T>::get_depth() const {
  return get_step().depth_;
}

template <class T>
const char * tree_it<T>::get_key() const {
  return get_step().key_;
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

} // namespace art

#endif
