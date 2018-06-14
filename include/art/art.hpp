/**
 * @file adaptive radix tree header.
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_ART_HPP
#define ART_ART_HPP

#include "node.hpp"
#include "node_0.hpp"
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
  node<T> *node = this->root;
  int depth = 0;
  for (;;) {
    if (node == nullptr) {
      return nullptr;
    }
    const key_type prefix = node->get_prefix();
    const int prefix_len = prefix.size();
    const int prefix_match_len = node->check_prefix(key, depth);
    const bool prefix_match = prefix_len == prefix_match_len;
    if (!prefix_match) {
      return nullptr;
    }
    if (prefix_len == key.size() - depth) {
      /* exact match */
      return node->get_value();
    }
    depth += prefix_len;
    node = node->find_child(key[depth]);
    depth += 1;
  }
}

// TODO(rafaelkallis): test
template <class T> T *art<T>::set(const key_type &key, T *value) {
  if (this->root == nullptr) {
    /* root is empty */
    this->root = new node_0<T>(key, value);
    return nullptr;
  }

  /* pointer to current node */
  node<T> *cur = this->root;

  /* pointer to previous node, i.e. current node's parent */
  node<T> *prev = nullptr;

  /* the partial key used to advande from the previous to the current node */
  partial_key_type cur_partial_key = 0;

  /* current key depth */
  int depth = 0;

  /* length of the key */
  const int key_len = key.size();

  for (;;) {
    /* prefix of the current node */
    const key_type prefix = cur->get_prefix();

    /* length of the prefix of the current node */
    const int prefix_len = prefix.size();

    /* number of bytes of the current node's prefix that match the key */
    const int prefix_match_len = cur->check_prefix(key, depth);

    /* true if the current node's prefix matches with a part of the key */
    const bool prefix_match =
        min(prefix_len, key_len - depth) == prefix_match_len;

    if (prefix_match && prefix_len == key_len - depth) {
      /* exact match:
       * => replace value of current node.
       *
       *        |                             |
       *       (aa)->Ø                       (aa)->Ø
       *    a /    \ b     +[aaaaa,v3]    a /    \ b
       *     /      \      ==========>     /      \
       * *(aa)->v1  ()->v2             *(aa)->v3  ()->v2
       *  /|\      /|\                  /|\      /|\
       */

      T *old_value = cur->get_value();
      cur->set_value(value);
      return old_value;
    }

    if (prefix_match && prefix_len > key_len - depth) {
      /* new key is a prefix of the current node's key:
       * => new value becomes parent of leaf node.
       *
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

      key_type new_node_prefix =
          key_type(prefix.cbegin(), prefix.cbegin() + prefix_match_len);
      node<T> *new_node = new node_4<T>(new_node_prefix, value);
      cur->set_prefix(
          key_type(prefix.cbegin() + prefix_match_len + 1, prefix.cend()));
      new_node->set_child(prefix[prefix_match_len], cur);

      if (prev == nullptr) {
        this->root = new_node;
      } else {
        prev->set_child(cur_partial_key, new_node);
      }
      return nullptr;
    }

    if (cur->is_leaf()) {
      /* lazy expansion */

      if (prefix_match && prefix_len < key_len - depth) {
        /* current node's key is a prefix of the new node's key:
         * => new node with new value becomes child of current node.
         *
         *       (aa)->Ø                        (aa)->Ø
         *    a /    \ b     +[aaaaaa,v3]    a /    \ b
         *     /      \      ===========>     /      \
         * *(aa)->v1  ()->v2              *(aa)->v1  ()->v2
         *                              a /
         *                               /
         *                             +()->v3
         */

        if (cur->is_full()) {
          cur = cur->grow();
          if (prev == nullptr) {
            this->root = cur;
          } else {
            prev->set_child(cur_partial_key, cur);
          }
        }
        key_type new_node_prefix =
            key_type(key.begin() + depth + prefix_len + 1, key.end());
        node_0<T> *new_node = new node_0<T>(new_node_prefix, value);
        cur->set_child(key[depth + prefix_len], new_node);
        return nullptr;
      }

      /* leaf node prefix mismatch:
       * => new parent node in place of old leaf node.
       * => old leaf and new node with value become children of new parent
       *    node.
       *
       *       (aa)->Ø                      (aa)->Ø
       *    a /    \ b     +[aaaab,v3]   a /    \ b
       *     /      \      ==========>    /      \
       *  (aa)->v1  ()->v2             +(a)->Ø   ()->v2
       *                             a /   \ b
       *                              /     \
       *                            ()->v1 +()->v3
       */

      key_type new_parent_prefix =
          key_type(prefix.begin(), prefix.begin() + prefix_match_len);
      node<T> *new_parent = new node_4<T>(new_parent_prefix, nullptr);

      cur->set_prefix(
          key_type(prefix.begin() + prefix_match_len + 1, prefix.end()));
      new_parent->set_child(prefix[prefix_match_len + 1], cur);

      key_type new_node_prefix =
          key_type(key.begin() + depth + prefix_match_len + 1, key.end());
      node_0<T> *new_node = new node_0<T>(new_node_prefix, value);
      new_parent->set_child(key[depth + prefix_match_len], new_node);

      if (prev == nullptr) {
        this->root = new_parent;
      } else {
        prev->set_child(cur_partial_key, new_parent);
      }
      return nullptr;
    }

    if (!prefix_match) {
      /* path compression:
       * => 1 new parent node in place of old internal node.
       * => old internal node and new node with inserted value become children
       *    of new parent node.
       *
       *      *(aa)->Ø                 +(a)->Ø
       *    a /    \ b     +[ab,v3]  a /   \ b
       *     /      \      =======>   /     \
       *  (aa)->v1  ()->v2          *()->Ø +()->v3
       *                          a /   \ b
       *                           /     \
       *                        (aa)->v1 ()->v2
       */

      key_type new_parent_prefix =
          key_type(prefix.begin(), prefix.begin() + prefix_match_len);
      node<T> *new_parent = new node_4<T>(new_parent_prefix, nullptr);

      cur->set_prefix(
          key_type(prefix.begin() + prefix_match_len + 1, prefix.end()));
      new_parent->set_child(prefix[prefix_match_len + 1], cur);

      key_type new_node_prefix =
          key_type(key.begin() + depth + prefix_match_len + 1, key.end());
      node_0<T> *new_node = new node_0<T>(new_node_prefix, value);
      new_parent->set_child(key[depth + prefix_match_len], new_node);

      if (prev == nullptr) {
        this->root = new_parent;
      } else {
        prev->set_child(cur_partial_key, new_parent);
      }
      return nullptr;
    }

    const partial_key_type next_partial_key = key[depth + prefix_len];
    node<T> *next = cur->find_child(next_partial_key);

    if (next == nullptr) {
      /*
       * No child associated with the next partial key.
       * => new node with the value becomes current node's child.
       *
       *      *(aa)->Ø              *(aa)->Ø
       *    a /        +[aab,v2]  a /    \ b
       *     /         ========>   /      \
       *   (a)->v1               (a)->v1 +()->v2
       */

      if (cur->is_full()) {
        cur = cur->grow();
        if (prev == nullptr) {
          this->root = cur;
        } else {
          prev->set_child(cur_partial_key, cur);
        }
      }
      key_type new_node_prefix = key_type(key.begin() + depth + 1, key.end());
      node<T> *new_node = new node_0<T>(new_node_prefix, value);
      cur->set_child(next_partial_key, new_node);
      return nullptr;
    }

    /* propagate down:
     *
     *     *(aa)->Ø                   (aa)->Ø
     *   a /    \ b    +[aaba,v3]  a /    \ b     repeat
     *    /      \     =========>   /      \     ========>  ...
     *  (a)->v1  ()->v2           (a)->v1 *()->v2
     */

    prev = cur;
    cur_partial_key = next_partial_key;
    cur = next;
    depth += prefix_len + 1;
  }
}

} // namespace art

#endif
