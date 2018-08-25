/**
 * @file trie nodes header.
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_HPP
#define ART_NODE_HPP

#include "child_it.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iterator>
#include <stdexcept>

namespace art {

template <class T> class node {
public:
  virtual ~node() = default;

  node() = default;
  node(const node<T> &other) = default;
  node(node<T> &&other) noexcept = default;
  node<T> &operator=(const node<T> &other) = default;
  node<T> &operator=(node<T> &&other) noexcept = default;

  /**
   * Finds and returns the child node identified by the given partial key.
   *
   * @param partial_key - The partial key associated with the child.
   * @return Child node identified by the given partial key or
   * a null pointer of no child node is associated with the partial key.
   */
  virtual node<T> **find_child(char partial_key) = 0;

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
  virtual void set_child(char partial_key, node<T> *child) = 0;

  /**
   * Deletes the child associated with the given partial key.
   *
   * @param partial_key - The partial key associated with the child.
   */
  virtual node<T> *del_child(char partial_key) = 0;

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
  int check_prefix(const char *key, int key_len) const;

  virtual int n_children() const = 0;

  virtual char next_partial_key(char partial_key) const = 0;

  virtual char prev_partial_key(char partial_key) const = 0;

  /**
   * Iterator on the first child node.
   *
   * @return Iterator on the first child node.
   */
  child_it<T> begin();
  std::reverse_iterator<child_it<T>> rbegin();
  child_it<T> end();
  std::reverse_iterator<child_it<T>> rend();

  char *prefix_ = nullptr;
  uint16_t prefix_len_ = 0;
  T *value_ = nullptr;
};

template <class T>
int node<T>::check_prefix(const char *key, int /* key_len */) const {
  // TODO(rafaelkallis): && i < key_len ??
  for (int i = 0; i < prefix_len_; ++i) {
    if (prefix_[i] != key[i]) {
      return i;
    }
  }
  return prefix_len_;
}

template <class T> child_it<T> node<T>::begin() { return child_it<T>(this); }

template <class T> std::reverse_iterator<child_it<T>> node<T>::rbegin() {
  return std::reverse_iterator<child_it<T>>(end());
}

template <class T> child_it<T> node<T>::end() {
  return child_it<T>(this, n_children());
}

template <class T> std::reverse_iterator<child_it<T>> node<T>::rend() {
  return std::reverse_iterator<child_it<T>>(begin());
}

} // namespace art

#endif
