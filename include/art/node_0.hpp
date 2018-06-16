/**
 * @file node_0 header
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_0_HPP
#define ART_NODE_0_HPP

#include "node.hpp"
#include "node_4.hpp"
#include <stdexcept>
#include <utility>

namespace art {

using std::out_of_range;

template <class T> class node_0 : public node<T> {
public:
  node_0() = default;
  node_0(key_type prefix, T *value);

  node<T> **find_child(const partial_key_type &partial_key) override;
  void set_child(const partial_key_type &partial_key, node<T> *child) override;
  node<T> *grow() override;
  bool is_full() const override;
  bool is_leaf() const override;

  partial_key_type next_partial_key(
      const partial_key_type &partial_key) noexcept(false) override;
};

template <class T>
node_0<T>::node_0(key_type prefix, T *value) : node<T>(prefix, value) {}

template <class T>
node<T> **node_0<T>::find_child(const partial_key_type & /* partial_key */) {
  return nullptr;
}

template <class T>
void node_0<T>::set_child(const partial_key_type & /* partial_key */,
                          node<T> * /* child */) {}

template <class T> node<T> *node_0<T>::grow() {
  auto new_node = new node_4<T>(this->get_prefix(), this->get_value());
  delete this;
  return new_node;
}

template <class T> bool node_0<T>::is_full() const { return true; }

template <class T> bool node_0<T>::is_leaf() const { return true; }

template <class T>
partial_key_type node_0<T>::next_partial_key(
    const partial_key_type &partial_key) noexcept(false) {
  throw out_of_range("provided partial key does not have a successor");
}

} // namespace art

#endif
