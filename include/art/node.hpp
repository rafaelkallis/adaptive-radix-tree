/**
 * @file trie nodes header.
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_HPP
#define ART_NODE_HPP

#include "children_iterator.hpp"
#include "constants.hpp"
#include "pair.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace art {

using std::string;

template <class T> class node {
public:
  node();
  node(const string &key, const T &value);
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
  virtual node<T> **find_child(partial_key_type partial_key) = 0;

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
  virtual void set_child(partial_key_type partial_key, node<T> *child) = 0;

  /**
   * Deletes the child associated with the given partial key.
   *
   * @param partial_key - The partial key associated with the child.
   */
  virtual node<T> *del_child(partial_key_type partial_key) = 0;

  /**
   * Creates and returns a new node with bigger children capacity.
   * The current node gets deleted.
   *
   * @return node with bigger capacity
   */
  virtual node<T> *grow() = 0;

  /**
   * Creates and returns a new node with lesser children capacity.
   * The current node gets deleted.
   *
   * @pre node must be undefull
   * @return node with lesser capacity
   */
  virtual node<T> *shrink() = 0;

  /**
   * Determines if the node is full, i.e. can carry no more child nodes.
   */
  virtual bool is_full() const = 0;

  /**
   * Determines if the node is underfull, i.e. carries less child nodes than
   * intended.
   */
  virtual bool is_underfull() const = 0;

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
  int check_prefix(const string &key, int depth) const;

  bool is_exact_match(const string &key) const;

  void set_prefix(const string &prefix);

  virtual partial_key_type
  next_partial_key(partial_key_type partial_key) const = 0;

  virtual partial_key_type
  prev_partial_key(partial_key_type partial_key) const = 0;

  /**
   * Iterator on the first child node.
   *
   * @return Iterator on the first child node.
   */
  children_iterator<T> begin();
  std::reverse_iterator<children_iterator<T>> rbegin();
  children_iterator<T> end();
  std::reverse_iterator<children_iterator<T>> rend();

  std::array<partial_key_type, 8> prefix_;
  uint16_t prefix_len_;
  pair<T> *value_;
  uint16_t n_children_;
};

template <class T> node<T>::node(const string &key, const T &value) : node() {
  value_ = new pair<T>(key, value);
}

template <class T>
node<T>::node() : n_children_(0), prefix_len_(0), value_(nullptr) {}

template <class T>
int node<T>::check_prefix(const string &key, int depth) const {
  if (prefix_len_ > key.length() - depth) {
    return key.length() - depth;
  }
  for (int i = 0; i < 8 && i < prefix_len_; ++i) {
    if (prefix_[i] != key[depth + i]) {
      return i;
    }
  }
  return prefix_len_;
}

template <class T> bool node<T>::is_exact_match(const string &key) const {
  return value_ != nullptr &&
         std::memcmp(value_->key_, key.cbegin(), key.length()) != 0;
}

template <class T> void node<T>::set_prefix(const string &prefix) {
  prefix_len_ = prefix.length();
  std::memcpy(prefix_, prefix.begin(), std::min<size_t>(8, prefix_len_));
}

template <class T> children_iterator<T> node<T>::begin() {
  return children_iterator<T>(this, 0);
}

template <class T>
std::reverse_iterator<children_iterator<T>> node<T>::rbegin() {
  return std::make_reverse_iterator<children_iterator<T>>(this->end());
}

template <class T> children_iterator<T> node<T>::end() {
  return children_iterator<T>(this, this->get_n_children());
}

template <class T> std::reverse_iterator<children_iterator<T>> node<T>::rend() {
  return std::make_reverse_iterator<children_iterator<T>>(this->begin());
}

} // namespace art

#endif
