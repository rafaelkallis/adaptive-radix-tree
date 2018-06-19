/**
 * @file trie nodes header.
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_HPP
#define ART_NODE_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace art {

using std::make_reverse_iterator;
using std::out_of_range;
using std::reverse_iterator;
using std::vector;

using partial_key_type = uint8_t;
using key_type = vector<partial_key_type>;

template <class T> class node {
public:
  node() = default;
  node(key_type prefix, T *value);
  node(const node<T> &other) = default;
  node(node<T> &&other) noexcept = default;
  virtual ~node() = default;

  node<T> &operator=(const node<T> &other) = default;
  node<T> &operator=(node<T> &&other) noexcept = default;

  /**
   * Finds and returns the child node identified by the given partial key.
   *
   * @param partial_key - The partial key associated with the child.
   * @return Child node identified by the given partial key or
   * a null pointer of no child node is associated with the partial key.
   */
  virtual node<T> **find_child(const partial_key_type &partial_key) = 0;

  /**
   * Adds the given node to the node's children.
   * No bounds checking is done.
   * If a child already exists under the given partial key, the child
   * is overwritten without deleting it.
   *
   * @pre Node should not be full.
   * @param partial_key - The partial key associated with the child.
   * @param child - The child node.
   */
  virtual void set_child(const partial_key_type &partial_key,
                         node<T> *child) = 0;

  /**
   * Creates and returns a new node with bigger children capacity.
   * The current node gets deleted.
   *
   * @return
   */
  virtual node<T> *grow() = 0;

  /**
   * Determines if the node is full, i.e. can carry no more child nodes.
   */
  virtual bool is_full() const = 0;

  /**
   * Determines if the node is a leaf node.
   */
  virtual bool is_leaf() const = 0;

  /**
   * Determines the number of matching bytes between the node's prefix and
   * the key. The first byte we compare is prefix[0] and key[depth + 0].
   *
   * Given a node with prefix: "abbd", a key "aaabbbccc" and depth 2,
   * checkPrefix returns 4, since the (5)th byte of the prefix ('c') does not
   * match the (depth + 5)th byte of the key ('d').
   *
   * key:     "aaabbbccc"
   * prefix:    "abbbd"
   *             ^^^^*
   */
  int check_prefix(const key_type &key, int depth) const;

  T *get_value() const;
  void set_value(T *value);
  key_type get_prefix() const;
  void set_prefix(const key_type &prefix);

  virtual int get_n_children() const = 0;

  virtual partial_key_type next_partial_key(partial_key_type partial_key) const
      noexcept(false) = 0;

  virtual partial_key_type prev_partial_key(partial_key_type partial_key) const
      noexcept(false) = 0;

  class iterator {
  public:
    iterator(node<T> *n, int relative_index);

    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = const partial_key_type;
    using difference_type = int;
    using pointer = value_type *;
    using reference = value_type &;

    reference operator*() const;
    pointer operator->() const;
    node<T>::iterator &operator++();
    node<T>::iterator operator++(int);
    node<T>::iterator &operator--();
    node<T>::iterator operator--(int);
    bool operator==(const node<T>::iterator &rhs) const;
    bool operator!=(const node<T>::iterator &rhs) const;
    bool operator<(const node<T>::iterator &rhs) const;
    bool operator>(const node<T>::iterator &rhs) const;
    bool operator<=(const node<T>::iterator &rhs) const;
    bool operator>=(const node<T>::iterator &rhs) const;

  private:
    node<T> *node_;
    partial_key_type cur_partial_key_;
    int relative_index_;
  };

  /**
   * Iterator on the first child node.
   *
   * @return Iterator on the first child node.
   */
  iterator begin();
  reverse_iterator<iterator> rbegin();
  iterator end();
  reverse_iterator<iterator> rend();

private:
  key_type prefix_ = key_type(0);
  T *value_ = nullptr;
};

template <class T>
node<T>::node(key_type prefix, T *value)
    : prefix_(move(prefix)), value_(value){};

template <class T>
int node<T>::check_prefix(const key_type &key, int depth) const {
  auto prefix_len = this->prefix_.size();
  for (int i = 0; i < prefix_len; i += 1) {
    if (this->prefix_[i] != key[depth + i]) {
      return i;
    }
  }
  return prefix_len;
}

template <class T> T *node<T>::get_value() const { return this->value_; }

template <class T> void node<T>::set_value(T *value) { this->value_ = value; }

template <class T> key_type node<T>::get_prefix() const {
  return this->prefix_;
}

template <class T> void node<T>::set_prefix(const key_type &prefix) {
  this->prefix_ = prefix;
}

template <class T> typename node<T>::iterator node<T>::begin() {
  return node<T>::iterator(this, 0);
}

template <class T>
reverse_iterator<typename node<T>::iterator> node<T>::rbegin() {
  return std::make_reverse_iterator<node<T>::iterator>(this->end());
}

template <class T> typename node<T>::iterator node<T>::end() {
  return node<T>::iterator(this, this->get_n_children());
}

template <class T>
reverse_iterator<typename node<T>::iterator> node<T>::rend() {
  return std::make_reverse_iterator<node<T>::iterator>(this->begin());
}

template <class T>
node<T>::iterator::iterator(node<T> *n, int relative_index)
    : node_(n), cur_partial_key_(0), relative_index_(relative_index) {
  if (relative_index_ < 0) {
    /* relative_index is out of bounds, no seek */
    return;
  }
  if (relative_index_ >= node_->get_n_children()) {
    /* relative_index is out of bounds, no seek */
    return;
  }

  /* relative_index is in bounds, (forward) seek next partial key */
  for (int i = 0; i < relative_index_; ++i) {
    cur_partial_key_ = node_->next_partial_key(cur_partial_key_);
  }
}

template <class T>
typename node<T>::iterator::reference node<T>::iterator::operator*() const {
  return cur_partial_key_;
}

template <class T>
typename node<T>::iterator::pointer node<T>::iterator::operator->() const {
  return &cur_partial_key_;
}

template <class T> typename node<T>::iterator &node<T>::iterator::operator++() {
  ++relative_index_;
  if (relative_index_ == 0) {
    cur_partial_key_ = node_->next_partial_key(0);
  } else if (relative_index_ < node_->get_n_children()) {
    cur_partial_key_ = node_->next_partial_key(cur_partial_key_ + 1);
  }
  return *this;
}

template <class T>
typename node<T>::iterator node<T>::iterator::operator++(int) {
  auto old = *this;
  operator++();
  return old;
}

template <class T> typename node<T>::iterator &node<T>::iterator::operator--() {
  --relative_index_;
  if (relative_index_ == node_->get_n_children() - 1) {
    cur_partial_key_ = node_->prev_partial_key(255);
  } else if (relative_index_ >= 0) {
    cur_partial_key_ = node_->prev_partial_key(cur_partial_key_ - 1);
  }
  return *this;
}

template <class T>
typename node<T>::iterator node<T>::iterator::operator--(int) {
  auto old = *this;
  operator--();
  return old;
}

template <class T>
bool node<T>::iterator::
operator==(const typename node<T>::iterator &rhs) const {
  /* should be from same node */
  assert(node_ == rhs.node_);
  return relative_index_ == rhs.relative_index_;
}

template <class T>
bool node<T>::iterator::operator<(const typename node<T>::iterator &rhs) const {
  /* should be from same node */
  assert(node_ == rhs.node_);
  return relative_index_ < rhs.relative_index_;
}

template <class T>
bool node<T>::iterator::
operator!=(const typename node<T>::iterator &rhs) const {
  return !((*this) == rhs);
}

template <class T>
bool node<T>::iterator::
operator>=(const typename node<T>::iterator &rhs) const {
  return !((*this) < rhs);
}

template <class T>
bool node<T>::iterator::
operator<=(const typename node<T>::iterator &rhs) const {
  return (rhs >= (*this));
}

template <class T>
bool node<T>::iterator::operator>(const typename node<T>::iterator &rhs) const {
  return (rhs < (*this));
}

} // namespace art

#endif
