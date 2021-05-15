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
  node() = default;
  explicit node(uint16_t prefix_len);
  node(const char *prefix, uint16_t prefix_len);
  node(const node<T> &other);
  node(node<T> &&other) noexcept;
  virtual ~node();
  
  node<T> &operator=(const node<T> &other);
  node<T> &operator=(node<T> &&other) noexcept;

  /**
   * Determines if this node is a leaf node, i.e., contains a value.
   * Needed for downcasting a node<T> instance to a leaf_node<T> or inner_node<T> instance.
   */
  virtual bool is_leaf() const = 0;

  /**
   * Determines the number of matching bytes between the node's prefix and the key.
   *
   * Given a node with prefix: "abbbd", a key "abbbccc",
   * check_prefix returns 4, since byte 4 of the prefix ('d') does not
   * match byte 4 of the key ('c').
   *
   * key:     "abbbccc"
   * prefix:  "abbbd"
   *           ^^^^*
   * index:    01234
   */
  int check_prefix(const char *key, int key_len) const;

  char *prefix_ = nullptr;
  uint16_t prefix_len_ = 0;
};

template <class T>
node<T>::node(uint16_t prefix_len) : prefix_(prefix_len ? new char[prefix_len] : nullptr), prefix_len_(prefix_len) {}

template <class T>
node<T>::node(const char *prefix, uint16_t prefix_len) : node(prefix_len) {
  std::copy_n(prefix, prefix_len, prefix_);
}

template <class T>
node<T>::node(const node<T> &other) : node(other.prefix_, other.prefix_len_) {}

template <class T>
node<T>::node(node<T> &&other) noexcept 
  : prefix_(other.prefix_),
    prefix_len_(other.prefix_len_) {
      other.prefix_ = nullptr;
      other.prefix_len_ = 0;
    }

template <class T>
node<T>::~node() {
  delete [] prefix_;
}

template <class T>
node<T>& node<T>::operator=(const node<T> &other) {
  if (this != &other) {
    char *prefix = other.prefix_len_ ? new char[other.prefix_len_] : nullptr;
    std::copy_n(other.prefix_, other.prefix_len_, prefix);
    uint16_t prefix_len = other.prefix_len_;

    delete [] prefix_;
    prefix_ = prefix;
    prefix_len_ = prefix_len;
  }
  return *this;
}

template <class T>
node<T>& node<T>::operator=(node<T> &&other) noexcept {
  if (this != &other) {
    delete [] prefix_;
    prefix_ = other.prefix_;
    other.prefix_ = nullptr;

    prefix_len_ = other.prefix_len_;
    other.prefix_len_ = 0;
  }
  return *this;
}

template <class T>
int node<T>::check_prefix(const char *key, int /* key_len */) const {
  return std::mismatch(prefix_, prefix_ + prefix_len_, key).second - key;
}

} // namespace art

#endif
