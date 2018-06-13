/**
 * @file trie nodes header.
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_HPP
#define ART_NODE_HPP

#include <algorithm>
#include <array>
#include <cstring>
#include <vector>

using std::array;
using std::vector;

/* namespace art { */

using partial_key_type = unsigned char;
using key_type = vector<partial_key_type>;

template <class T> class node {
public:
  node() = default;
  node(key_type prefix, T *value);
  node(const node<T> &other) = default;
  node(node<T> &&other) noexcept= default;
  virtual ~node() = default;

  node<T> &operator=(const node<T> &other) = default;
  node<T> &operator=(node<T> &&other) noexcept = default;

  /**
   * Finds and returns the child node identified by the given partial key.
   *
   * @param partial_key - The partial key associated with the child.
   * @return The child node identified by the given partial key or
   * a null pointer of no child node is associated with the partial key.
   */
  virtual node<T> *find_child(const partial_key_type &partial_key) const = 0;

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

/* } // namespace art */

#endif
