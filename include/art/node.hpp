/**
 * @file trie nodes header.
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_HPP
#define ART_NODE_HPP

#include "children_iterator.hpp"
#include "constants.hpp"
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
  using value_type = T *;
  using reference = value_type &;

  node() = default;
  node(const key_type &prefix, value_type value);
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
   * Deletes the child associated with the given partial key.
   *
   * @param partial_key - The partial key associated with the child.
   */
  virtual node<T> * del_child(const partial_key_type &partial_key) = 0;

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
   * Determines if the node is a leaf node.
   */
  bool is_leaf() const;

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

  reference get_value();
  void set_value(reference value);
  key_type get_prefix() const;
  void set_prefix(const key_type &prefix);

  virtual int get_n_children() const = 0;

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

private:
  key_type prefix_ = key_type(0);
  value_type value_ = nullptr;
};

template <class T>
node<T>::node(const key_type &prefix, value_type value)
    : prefix_(prefix), value_(value){};

template <class T>
int node<T>::check_prefix(const key_type &key, int depth) const {
  auto prefix_len = this->prefix_.size();
  for (int i = 0; i < prefix_len; ++i) {
    if (this->prefix_[i] != key[depth + i]) {
      return i;
    }
  }
  return prefix_len;
}

template <class T> bool node<T>::is_leaf() const {
  return this->get_n_children() == 0;
}

template <class T> typename node<T>::value_type &node<T>::get_value() {
  return this->value_;
}

template <class T> void node<T>::set_value(reference value) {
  this->value_ = value;
}

template <class T> key_type node<T>::get_prefix() const {
  return this->prefix_;
}

template <class T> void node<T>::set_prefix(const key_type &prefix) {
  this->prefix_ = prefix;
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
