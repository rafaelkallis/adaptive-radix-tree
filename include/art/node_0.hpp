/**
 * @file node_0 header
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_0_HPP
#define ART_NODE_0_HPP

#include "node.hpp"
#include <stdexcept>
#include <string>
#include <utility>

namespace art {

using std::string;

template <class T> class node_4;

template <class T> class node_0 : public node<T> {
public:
  node<T> **find_child(uint8_t partial_key) override;
  void set_child(uint8_t partial_key, node<T> *child) override;
  node<T> *del_child(uint8_t partial_key) override;
  node<T> *grow() override;
  node<T> *shrink() override;
  bool is_full() const override;
  bool is_underfull() const override;

  int n_children() const override;

  uint8_t next_partial_key(uint8_t partial_key) const override;

  uint8_t prev_partial_key(uint8_t partial_key) const override;
};

template <class T> node<T> **node_0<T>::find_child(uint8_t /* partial_key */) {
  return nullptr;
}

template <class T>
void node_0<T>::set_child(uint8_t /* partial_key */, node<T> * /* child */) {}

template <class T> node<T> *node_0<T>::del_child(uint8_t /* partial_key */) {
  return nullptr;
}

template <class T> node<T> *node_0<T>::grow() {
  auto new_node = new node_4<T>();
  new_node->prefix_ = this->prefix_;
  new_node->prefix_len_ = this->prefix_len_;
  new_node->value_ = this->value_;
  delete this;
  return new_node;
}

template <class T> node<T> *node_0<T>::shrink() {
  throw std::runtime_error("node_0 cannot shrink");
}

template <class T> bool node_0<T>::is_full() const { return true; }

template <class T> bool node_0<T>::is_underfull() const { return false; }

template <class T> int node_0<T>::n_children() const { return 0; }

template <class T>
uint8_t node_0<T>::next_partial_key(uint8_t /* partial_key */) const {
  throw std::out_of_range("provided partial key does not have a successor");
}

template <class T>
uint8_t node_0<T>::prev_partial_key(uint8_t /* partial_key */) const {
  throw std::out_of_range("provided partial key does not have a predecessor");
}

} // namespace art

#endif
