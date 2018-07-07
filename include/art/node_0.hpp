/**
 * @file node_0 header
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_0_HPP
#define ART_NODE_0_HPP

#include "node.hpp"
#include <stdexcept>
#include <utility>

namespace art {

template <class T> class node_4;

template <class T> class node_0 : public node<T> {
public:
  node_0() = default;
  node_0(const key_type &prefix, T *value);

  node<T> **find_child(const partial_key_type &partial_key) override;
  void set_child(const partial_key_type &partial_key, node<T> *child) override;
  node<T> * del_child(const partial_key_type &partial_key) override;
  node<T> *grow() override;
  node<T> *shrink() override;
  bool is_full() const override;
  bool is_underfull() const override;

  int get_n_children() const override;

  partial_key_type
  next_partial_key(partial_key_type partial_key) const override;

  partial_key_type
  prev_partial_key(partial_key_type partial_key) const override;
};

template <class T>
node_0<T>::node_0(const key_type &prefix, T *value) : node<T>(prefix, value) {}

template <class T>
node<T> **node_0<T>::find_child(const partial_key_type & /* partial_key */) {
  return nullptr;
}

template <class T>
void node_0<T>::set_child(const partial_key_type & /* partial_key */,
                          node<T> * /* child */) {}

template <class T>
node<T> * node_0<T>::del_child(const partial_key_type & /* partial_key */) {
  return nullptr;
}

template <class T> node<T> *node_0<T>::grow() {
  auto new_node = new node_4<T>(this->get_prefix(), this->get_value());
  delete this;
  return new_node;
}

template <class T> node<T> *node_0<T>::shrink() {
  throw std::runtime_error("node_0 cannot shrink");
}

template <class T> bool node_0<T>::is_full() const { return true; }

template <class T> bool node_0<T>::is_underfull() const { return false; }

template <class T> int node_0<T>::get_n_children() const { return 0; }

template <class T>
partial_key_type
node_0<T>::next_partial_key(partial_key_type /* partial_key */) const {
  throw std::out_of_range("provided partial key does not have a successor");
}

template <class T>
partial_key_type
node_0<T>::prev_partial_key(partial_key_type /* partial_key */) const {
  throw std::out_of_range("provided partial key does not have a predecessor");
}

} // namespace art

#endif
