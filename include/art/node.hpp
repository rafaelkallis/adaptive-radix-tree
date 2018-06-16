/**
 * @file trie nodes header.
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_HPP
#define ART_NODE_HPP

#include <algorithm>
#include <array>
#include <cstring>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace art {

using std::make_pair;
using std::out_of_range;
using std::pair;
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

  virtual partial_key_type
  next_partial_key(const partial_key_type &partial_key) noexcept(false) = 0;

  class iterator
      : public std::iterator<std::forward_iterator_tag,
                             pair<partial_key_type, node<T> *>, int64_t> {
  public:
    iterator(node<T> *n, uint8_t index, bool is_out_of_bounds);

    pair<partial_key_type, node<T> *> operator*();
    node<T>::iterator &operator++();
    node<T>::iterator operator++(int);
    bool operator==(const node<T>::iterator &rhs);
    bool operator!=(const node<T>::iterator &rhs);

  private:
    node<T> *n_;
    uint8_t index_;
    bool is_out_of_bounds_;
  };

  iterator begin();
  iterator end();

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
  return node<T>::iterator(this, 0, false);
}

template <class T> typename node<T>::iterator node<T>::end() {
  return node<T>::iterator(this, 0, true);
}

template <class T>
node<T>::iterator::iterator(node<T> *n, uint8_t index, bool is_out_of_bounds)
    : n_(n), index_(index), is_out_of_bounds_(is_out_of_bounds) {
  if (!this->is_out_of_bounds_) {
    try {
      this->index_ = this->n_->next_partial_key(this->index_);
    } catch (out_of_range &) {
      this->is_out_of_bounds_ = true;
    }
  }
}

template <class T>
pair<partial_key_type, node<T> *> node<T>::iterator::operator*() {
  return this->is_out_of_bounds_
             ? make_pair(0, nullptr)
             : make_pair(this->index_, *this->n_->find_child(this->index_));
}

template <class T> typename node<T>::iterator &node<T>::iterator::operator++() {
  try {
    this->index_ = this->n_->next_partial_key(this->index_ + 1);
  } catch (out_of_range &) {
    this->is_out_of_bounds_ = true;
  }
  return *this;
}

template <class T>
typename node<T>::iterator node<T>::iterator::operator++(int) {
  auto old_this = *this;
  ++(*this);
  return old_this;
}

template <class T>
bool node<T>::iterator::operator==(const node<T>::iterator &rhs) {
  return this->n_ == rhs.n_ &&
         ((this->is_out_of_bounds_ && rhs.is_out_of_bounds_) ||
          (!this->is_out_of_bounds_ && !rhs.is_out_of_bounds_ &&
           this->index_ == rhs.index_));
}

template <class T>
bool node<T>::iterator::operator!=(const node<T>::iterator &rhs) {
  return !((*this) == rhs);
}

} // namespace art

#endif
