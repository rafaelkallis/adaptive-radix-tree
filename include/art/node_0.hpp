/**
 * @file node_0 header
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_NODE_0_HPP
#define ART_NODE_0_HPP

#include "node.hpp"
#include "node_4.hpp"
#include <utility>

namespace art {

using std::make_pair;
using std::pair;

template <class T> class node_0 : public node<T> {
public:
  node_0() = default;
  node_0(key_type prefix, T *value);
  node_0(const node_0<T> &other) = default;
  node_0(node_0<T> &&other) noexcept = default;
  ~node_0() = default;

  node_0<T> &operator=(const node_0<T> &other) = default;
  node_0<T> &operator=(node_0<T> &&other) noexcept = default;

  node<T> **find_child(const partial_key_type &partial_key) override;
  void set_child(const partial_key_type &partial_key, node<T> *child) override;
  node<T> *grow() override;
  bool is_full() const override;
  bool is_leaf() const override;

  typename node<T>::iterator begin() override;
  typename node<T>::iterator end() override;

  class iterator : public node<T>::iterator {
  public:
    explicit iterator(node_0<T> *node);

    pair<partial_key_type, node<T> *> operator*();
    iterator &operator++();
    bool operator==(const node_0<T>::iterator &rhs);
    bool operator!=(const node_0<T>::iterator &rhs);

  private:
    node_0<T> *node_;
  };
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

template <class T> typename node<T>::iterator node_0<T>::begin() {
  return node_0<T>::iterator(this);
}

template <class T> typename node<T>::iterator node_0<T>::end() {
  return node_0<T>::iterator(this);
}

template <class T>
node_0<T>::iterator::iterator(node_0<T> *node)
    : node_(node) {}


template <class T>
pair<partial_key_type, node<T> *> node_0<T>::iterator::operator*() {
  return make_pair(0, nullptr);
}

template <class T>
typename node_0<T>::iterator &node_0<T>::iterator::operator++() {
  return *this;
}

template <class T>
bool node_0<T>::iterator::operator==(const node_0<T>::iterator &rhs) {
  return this->node_ == rhs.node_;
}

template <class T>
bool node_0<T>::iterator::operator!=(const node_0<T>::iterator &rhs) {
  return !((*this) == rhs);
}

} // namespace art

#endif
