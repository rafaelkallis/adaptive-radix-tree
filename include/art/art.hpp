/**
 * @file adaptive radix tree header.
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_ART_HPP
#define ART_ART_HPP

#include "node.hpp"
#include "node_0.hpp"
#include "node_4.hpp"
#include "preorder_traversal_iterator.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <stack>
#include <string>
#include <utility>

namespace art {

using std::string;

template <class T> class art {
public:
  ~art();

  /**
   * Finds the value associated with the given key.
   *
   * @param key - The key to find.
   * @return the value associated with the key or a nullptr.
   */
  const T &get(const string &key) const;

  /**
   * Associates the given key with the given value.
   * If another value is already associated with the given key,
   * since the method consumer is the resource owner.
   *
   * @param key - The key to associate with the value.
   * @param value - The value to be associated with the key.
   * @return a nullptr if no other value is associated with they or the
   * previously associated value.
   */
  const T &set(const string &key, const T &value);

  /**
   * Deletes the given key and returns it's associated value.
   * The associated value is returned,
   * since the method consumer is the resource owner.
   * If no value is associated with the given key, nullptr is returned.
   *
   * @param key - The key to delete.
   * @return the values assciated with they key or a nullptr otherwise.
   */
  const T &del(const string &key);

  /**
   * Forward iterator that traverses the tree in lexicographic order.
   */
  preorder_traversal_iterator<T> lexicographic_it();

  /**
   * Forward iterator that traverses the tree in lexicographic order starting
   * from the provided key.
   */
  preorder_traversal_iterator<T> lexicographic_it(const string &key);

  /**
   * Iterator to the end of the lexicographic order.
   */
  preorder_traversal_iterator<T> lexicographic_it_end();

private:
  node<T> *root_ = nullptr;
};

template <class T> art<T>::~art() {
  if (root_ == nullptr) {
    return;
  }
  std::stack<node<T> *> node_stack;
  node_stack.push(root_);
  while (!node_stack.empty()) {
    node<T> *cur = node_stack.top();
    node_stack.pop();
    for (auto child_it = cur->rbegin(), child_it_end = cur->rend();
         child_it != child_it_end; ++child_it) {
      auto partial_key = *child_it;
      node_stack.push(*cur->find_child(partial_key));
    }
    delete cur;
  }
}

template <class T> const T &art<T>::get(const string &key) const {
  node<T> *cur = root_, **child;
  int depth = 0, prefix_len, key_len = key.length();
  while (cur != nullptr) {
    prefix_len = cur->get_prefix_len();
    if (prefix_len != cur->check_prefix(key, depth)) {
      /* prefix mismatch */
      return nullptr;
    }
    if (prefix_len == key_len - depth) {
      /* exact match */
      if (std::memcmp(cur->value_.key_, key, key_len) != 0) {
        /* key mismatch */
        return nullptr;
      }
      return cur->value_.value_;
    }
    depth += prefix_len;
    child = cur->find_child(key[depth]);
    cur = child != nullptr ? *child : nullptr;
    ++depth;
  }
  return nullptr;
}

template <class T> const T &art<T>::set(const string &key, const T &value) {

  /* length of the key */
  const int key_len = key.length();

  if (root_ == nullptr) {
    root_ = new node_0<T>(key, value);
    root_->set_prefix(key);
    return nullptr;
  }

  node<T> **cur, **child;
  ;
  int depth, prefix_len, prefix_match_len;
  partial_key_type cur_partial_key, child_partial_key;
  bool is_prefix_match, is_exact_match;

  /* pointer to current node */
  cur = &root_;

  /* current key depth */
  depth = 0;

  while (true) {
    /* length of the current prefix */
    prefix_len = (*cur)->prefix_len_;

    /* number of bytes of the current node's prefix that match the key */
    prefix_match_len = (*cur)->check_prefix(key, depth);

    /* true if the current node's prefix matches with a part of the key */
    is_prefix_match =
        std::min((*cur)->prefix_len_, key_len - depth) == prefix_match_len;

    is_exact_match = is_prefix_match && prefix_len == key_len - depth;

    if (is_exact_match) {
      /* exact match:
       * => "replace"
       * => replace value of current node.
       * => return old value to caller to handle.
       *        _                             _
       *        |                             |
       *       (aa)->Ø                       (aa)->Ø
       *    a /    \ b     +[aaaaa,v3]    a /    \ b
       *     /      \      ==========>     /      \
       * *(aa)->v1  ()->v2             *(aa)->v3  ()->v2
       *  /|\       /|\                 /|\       /|\
       */

      if ((*cur)->is_exact_match(key)) {
        /* extra check due to optimistic compression */
        return nullptr;
      }

      auto old_value = (*cur)->value_.value_;
      (*cur)->value_.value_ = value;
      return old_value;
    }

    if (is_prefix_match && prefix_len > key_len - depth) {
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

      node<T> *new_node = new node_4<T>(key, value);
      new_node->set_prefix(key.substr(depth, prefix_match_len));
      (*cur)->set_prefix(key.substr(depth + prefix_match_len + 1,
                                    prefix_len - prefix_match_len - 1));
      new_node->set_child(key[depth + prefix_match_len], *cur);
      *cur = new_node;
      return nullptr;
    }

    if (!is_prefix_match) {
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

      node<T> *new_parent = new node_4<T>();
      new_parent->set_prefix(key.substr(depth, prefix_match_len));

      node<T> *new_node = new node_0<T>(key, value);
      new_node->set_prefix(key.substr(depth + prefix_match_len + 1,
                                      key_len - depth - prefix_match_len - 1));

      // TODO(rafaelkallis): get substing from prefix? search descendants?
      (*cur)->set_prefix(key.substr(depth + prefix_match_len + 1,
                                    prefix_len - prefix_match_len - 1));

      new_parent->set_child(cur_partial_key, *cur);
      new_parent->set_child(key[depth + prefix_match_len], new_node);

      *cur = new_parent;
      return nullptr;
    }

    child_partial_key = key[depth + prefix_len];
    child = (*cur)->find_child(child_partial_key);

    if (child == nullptr) {
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

    cur = child;
    cur_partial_key = child_partial_key;
    depth += prefix_len + 1;
  }
}

template <class T> T *art<T>::del(const key_type &key) {
  /* pointer to current node */
  node<T> **cur = &root_;

  partial_key_type cur_partial_key = 0;

  /* pointer to parent/previous node */
  node<T> **prev = nullptr;

  /* current key depth */
  int depth = 0;

  /* length of the key */
  const int key_len = key.length();

  while (*cur != nullptr) {
    /* prefix of the current node */
    const key_type prefix = (*cur)->get_prefix();

    /* length of the prefix of the current node */
    const int prefix_len = prefix.length();

    /* true if the current node's prefix matches with the key */
    const bool is_prefix_match = prefix_len == (*cur)->check_prefix(key, depth);

    if (!is_prefix_match) {
      /* prefix mismatch => key doesn't exist */
      return nullptr;
    }

    /* true if the current node's key matches the search key exactly */
    const bool is_exact_match = key_len - (depth + prefix_len) == 0;

    if (is_exact_match) {
      T *value = (*cur)->get_value();
      (*cur)->set_value(nullptr);
      const int n_children = (*cur)->get_n_children();
      const int n_siblings =
          prev != nullptr ? (*prev)->get_n_children() - 1 : 0;
      if (n_children == 0 && n_siblings == 0) {
        /*
         * => delete leaf node
         *
         *     |                 |
         *    (aa)->v1          (aa)->v1
         *     | a     -[aaaaa]
         *     |       =======>
         *   *(aa)->v2
         */

        delete (*cur);
        if (prev != nullptr) {
          (*prev)->del_child(cur_partial_key);
        } else {
          *cur = nullptr;
        }

      } else if (n_children == 0 && n_siblings == 1 &&
                 (*prev)->get_value() == nullptr) {
        /* => delete leaf node
         * => replace parent with sibling
         *
         *        |a                         |a
         *        |                          |
         *       (aa)->Ø     -"aaaaabaa"     |
         *    a /    \ b     ==========>    /
         *     /      \                    /
         *  (aa)->v1 *()->v2             (aaaaa)->v1
         *  /|\
         */

        /* find sibling */
        partial_key_type sibling_partial_key = (*prev)->next_partial_key(0);
        if (sibling_partial_key == cur_partial_key) {
          sibling_partial_key = (*prev)->next_partial_key(cur_partial_key + 1);
        }
        node<T> *sibling = *(*prev)->find_child(sibling_partial_key);

        const key_type new_sibling_prefix = (*prev)->get_prefix() +
                                            key_type(1, sibling_partial_key) +
                                            sibling->get_prefix();
        sibling->set_prefix(new_sibling_prefix);

        delete (*cur);
        delete (*prev);
        *prev = sibling;

      } else if (n_children == 0 && n_siblings > 1) {
        /* => delete leaf node
         *
         *        |a                         |a
         *        |                          |
         *       (aa)->Ø     -"aaaaabaa"    (aa)->Ø
         *    a / |  \ b     ==========> a / |
         *     /  |   \                   /  |
         *           *()->v1
         */

        delete (*cur);
        (*prev)->del_child(cur_partial_key);

      } else if (n_children == 1) {
        /* node with 1 child
         * => replace node with child
         *
         *       (aa)->v1            (aa)->v1
         *        |a                  |a
         *        |        -[aaaaa]   |
         *      *(aa)->v2  =======>   |
         *        |a                  |
         *        |                   |
         *       (aa)->v3            (aaaaa)->v3
         */

        const partial_key_type child_partial_key = (*cur)->next_partial_key(0);
        node<T> *child = *(*cur)->find_child(child_partial_key);
        const key_type new_child_prefix = (*cur)->get_prefix() +
                                          key_type(1, child_partial_key) +
                                          child->get_prefix();

        child->set_prefix(new_child_prefix);
        delete (*cur);
        *cur = child;
      }

      if (prev != nullptr && (*prev)->is_underfull()) {
        *prev = (*prev)->shrink();
      }
      return value;
    }

    /* propagate down and repeat */

    cur_partial_key = key[depth + prefix_len];
    prev = cur;
    cur = (*cur)->find_child(cur_partial_key);
    depth += prefix_len + 1;
  }
  return nullptr;
}

template <class T> preorder_traversal_iterator<T> art<T>::lexicographic_it() {
  return preorder_traversal_iterator<T>(root_);
}

template <class T>
preorder_traversal_iterator<T> art<T>::lexicographic_it(const key_type &key) {
  return preorder_traversal_iterator<T>::from_min_key(root_, key);
}

template <class T>
preorder_traversal_iterator<T> art<T>::lexicographic_it_end() {
  return preorder_traversal_iterator<T>();
}

} // namespace art

#endif
