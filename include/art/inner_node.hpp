/**
 * @file inner_node header
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_INNER_NODE_HPP
#define ART_INNER_NODE_HPP

#include "child_it.hpp"
#include "leaf_node.hpp"
#include "node.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iterator>
#include <stdexcept>

namespace art {

template <class T> class inner_node : public node<T> {
public:
  virtual ~inner_node() = default;

  inner_node() = default;
  inner_node(const inner_node<T> &other) = default;
  inner_node(inner_node<T> &&other) noexcept = default;
  inner_node<T> &operator=(const inner_node<T> &other) = default;
  inner_node<T> &operator=(inner_node<T> &&other) noexcept = default;

  bool is_leaf() const override;

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
  virtual inner_node<T> *grow() = 0;

  /**
   * Creates and returns a new node with lesser children capacity.
   * The current node gets deleted.
   *
   * @pre node must be undefull
   * @return node with lesser capacity
   */
  virtual inner_node<T> *shrink() = 0;

  /**
   * Determines if the node is full, i.e. can carry no more child nodes.
   */
  virtual bool is_full() const = 0;

  /**
   * Determines if the node is underfull, i.e. carries less child nodes than
   * intended.
   */
  virtual bool is_underfull() const = 0;

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

  /**
   * Iterator on after the last child node.
   *
   * @return Iterator on after the last child node.
   */
  child_it<T> end();
  std::reverse_iterator<child_it<T>> rend();
};

template <class T> bool inner_node<T>::is_leaf() const { return false; }

template <class T> child_it<T> inner_node<T>::begin() {
  return child_it<T>(this);
}

template <class T> std::reverse_iterator<child_it<T>> inner_node<T>::rbegin() {
  return std::reverse_iterator<child_it<T>>(end());
}

template <class T> child_it<T> inner_node<T>::end() {
  return child_it<T>(this, n_children());
}

template <class T> std::reverse_iterator<child_it<T>> inner_node<T>::rend() {
  return std::reverse_iterator<child_it<T>>(begin());
}

} // namespace art

#endif
