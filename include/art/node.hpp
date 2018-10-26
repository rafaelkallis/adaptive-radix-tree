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

  virtual bool is_leaf() const = 0;

  uint16_t prefix_len_ = 0;
};

} // namespace art

#endif
