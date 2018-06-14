/**
 * @file adaptive radix tree header.
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_ART_HPP
#define ART_ART_HPP

#include "node.hpp"
#include "node_0.hpp"
#include "node_4.hpp"
#include <algorithm>

namespace art {

using std::min;

template <class T> class art {
public:
  art() = default;
  ~art() = default;
  art(const art<T> &other) = default;
  art(art<T> &&other) noexcept = default;

  art<T> &operator=(const art<T> &other) = default;
  art<T> &operator=(art<T> &&other) noexcept = default;

  T *search(const key_type &key) const;
  T *set(const key_type &key, T *value);

private:
  node<T> *root = nullptr;
};

// TODO(rafaelkallis): test
template <class T> T *art<T>::search(const key_type &key) const {
  node<T> *cur = this->root;
  int depth = 0;
  for (;;) {
    if (cur == nullptr) {
      return nullptr;
    }
    const int prefix_len = cur->get_prefix().size();
    const bool prefix_match = cur->check_prefix(key, depth) == prefix_len;
    if (!prefix_match) {
      return nullptr;
    }
    if (prefix_len == key.size() - depth) {
      /* exact match */
      return cur->get_value();
    }
    depth += prefix_len;
    node<T> **next = cur->find_child(key[depth]);
    if (next != nullptr) {
      cur = *next;
    }
    depth += 1;
  }
}

// TODO(rafaelkallis): test
template <class T> T *art<T>::set(const key_type &key, T *value) {
  /* pointer to current node */
  node<T> **cur = &this->root;

  /* current key depth */
  int depth = 0;

  /* length of the key */
  const int key_len = key.size();

  for (;;) {
    if (*cur == nullptr) {
      *cur = new node_0<T>(key, value);
      return nullptr;
    }

    /* prefix of the current node */
    const key_type prefix = (*cur)->get_prefix();

    /* length of the prefix of the current node */
    const int prefix_len = prefix.size();

    /* number of bytes of the current node's prefix that match the key */
    const int prefix_match_len = (*cur)->check_prefix(key, depth);

    /* true if the current node's prefix matches with a part of the key */
    const bool prefix_match =
        min(prefix_len, key_len - depth) == prefix_match_len;

    if (prefix_match && prefix_len == key_len - depth) {
      /* exact match:
       * => "replace"
       * => replace value of current node.
       * => return old value for caller to handle.
       *        _                             _
       *        |                             |
       *       (aa)->Ø                       (aa)->Ø
       *    a /    \ b     +[aaaaa,v3]    a /    \ b
       *     /      \      ==========>     /      \
       * *(aa)->v1  ()->v2             *(aa)->v3  ()->v2
       *  /|\       /|\                 /|\       /|\
       */

      T *old_value = (*cur)->get_value();
      (*cur)->set_value(value);
      return old_value;
    }

    if (prefix_match && prefix_len > key_len - depth) {
      /* new key is a prefix of the current node's key:
       * => "expand"
       * => create new node with value to insert.
       * => new node becomes parent of current node.
       *        _                           _
       *        |                           |
       *       (aa)->Ø                     (aa)->Ø
       *    a /    \ b     +[aaaa,v3]   a /    \ b
       *     /      \      =========>    /      \
       * *(aa)->v1  ()->v2            +(a)->v3  ()->v2
       *  /|\       /|\             a /         /|\
       *                             /
       *                           *()->v1
       *                            /|\
       */

      const key_type new_node_prefix =
          key_type(prefix.cbegin(), prefix.cbegin() + prefix_match_len);
      node<T> *new_node = new node_4<T>(new_node_prefix, value);
      (*cur)->set_prefix(
          key_type(prefix.cbegin() + prefix_match_len + 1, prefix.cend()));
      new_node->set_child(prefix[prefix_match_len], *cur);
      *cur = new_node;
      return nullptr;
    }

    if (!prefix_match) {
      /* prefix mismatch:
       * => new parent node with common prefix and no associated value.
       * => new node with value to insert.
       * => current and new node become children of new parent node.
       *
       *        |                        |
       *      *(aa)->Ø                 +(a)->Ø
       *    a /    \ b     +[ab,v3]  a /   \ b
       *     /      \      =======>   /     \
       *  (aa)->v1  ()->v2          *()->Ø +()->v3
       *  /|\       /|\           a /   \ b
       *                           /     \
       *                        (aa)->v1 ()->v2
       *                        /|\      /|\
       */

      const key_type new_parent_prefix =
          key_type(prefix.cbegin(), prefix.cbegin() + prefix_match_len);
      node<T> *new_parent = new node_4<T>(new_parent_prefix, nullptr);

      (*cur)->set_prefix(
          key_type(prefix.cbegin() + prefix_match_len + 1, prefix.cend()));
      new_parent->set_child(prefix[prefix_match_len], *cur);

      const key_type new_node_prefix =
          key_type(key.cbegin() + depth + prefix_match_len + 1, key.cend());
      node<T> *new_node = new node_0<T>(new_node_prefix, value);
      new_parent->set_child(key[depth + prefix_match_len], new_node);

      *cur = new_parent;
      return nullptr;
    }

    const partial_key_type next_partial_key = key[depth + prefix_len];
    node<T> **next = (*cur)->find_child(next_partial_key);

    if (next == nullptr) {
      /*
       * no child associated with the next partial key.
       * => create new node with value to insert.
       * => new node becomes current node's child.
       *
       *      *(aa)->Ø              *(aa)->Ø
       *    a /        +[aab,v2]  a /    \ b
       *     /         ========>   /      \
       *   (a)->v1               (a)->v1 +()->v2
       */

      if ((*cur)->is_full()) {
        *cur = (*cur)->grow();
      }
      const key_type new_node_prefix =
          key_type(key.cbegin() + depth + prefix_len + 1, key.cend());
      node<T> *new_node = new node_0<T>(new_node_prefix, value);
      (*cur)->set_child(next_partial_key, new_node);
      return nullptr;
    }

    /* propagate down and repeat:
     *
     *     *(aa)->Ø                   (aa)->Ø
     *   a /    \ b    +[aaba,v3]  a /    \ b     repeat
     *    /      \     =========>   /      \     ========>  ...
     *  (a)->v1  ()->v2           (a)->v1 *()->v2
     */

    cur = next;
    depth += prefix_len + 1;
  }
}

} // namespace art

#endif
