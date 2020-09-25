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
   * Determines if this node is a leaf node, i.e., contains a value.
   * Needed for downcasting a node<T> instance to a leaf_node<T> or inner_node<T> instance.
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
  int check_prefix(const char *key, int key_len) const;

  char *prefix_ = nullptr;
  uint16_t prefix_len_ = 0;
};

template <class T>
int node<T>::check_prefix(const char *key, int /* key_len */) const {
  return std::mismatch(prefix_, prefix_ + prefix_len_, key).second - key;
}

} // namespace art

#endif
