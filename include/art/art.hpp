/**
 * @file adaptive radix tree
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_ART_HPP
#define ART_ART_HPP

#include "leaf_node.hpp"
#include "inner_node.hpp"
#include "node.hpp"
#include "node_4.hpp"
#include "tree_it.hpp"
#include <algorithm>
#include <iostream>
#include <stack>
#include <cstring>

namespace art {

template <class T> class art {
public:
  ~art();

  /**
   * Finds the value associated with the given key.
   *
   * @param key - The key to find.
   * @return the value associated with the key or a nullptr.
   */
  T *get(const char *key) const;

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
  T *set(const char *key, T *value);

  /**
   * Deletes the given key and returns it's associated value.
   * The associated value is returned,
   * since the method consumer is the resource owner.
   * If no value is associated with the given key, nullptr is returned.
   *
   * @param key - The key to delete.
   * @return the values assciated with they key or a nullptr otherwise.
   */
  T *del(const char *key);

  /**
   * Forward iterator that traverses the tree in lexicographic order.
   */
  tree_it<T> begin();

  /**
   * Forward iterator that traverses the tree in lexicographic order starting
   * from the provided key.
   */
  tree_it<T> begin(const char *key);

  /**
   * Iterator to the end of the lexicographic order.
   */
  tree_it<T> end();

private:
  node<T> *root_ = nullptr;
};

template <class T> art<T>::~art() {
  if (root_ == nullptr) {
    return;
  }
  std::stack<node<T> *> node_stack;
  node_stack.push(root_);
  node<T> *cur;
  inner_node<T> *cur_inner;
  child_it<T> it, it_end;
  while (!node_stack.empty()) {
    cur = node_stack.top();
    node_stack.pop();
    if (!cur->is_leaf()) {
      cur_inner = static_cast<inner_node<T>*>(cur);
      for (it = cur_inner->begin(), it_end = cur_inner->end(); it != it_end; ++it) {
        node_stack.push(*cur_inner->find_child(*it));
      }
    }
    if (cur->prefix_ != nullptr) {
      delete[] cur->prefix_;
    }
    delete cur;
  }
}

template <class T> T *art<T>::get(const char *key) const {
  node<T> *cur = root_, **child;
  int depth = 0, key_len = std::strlen(key) + 1;
  while (cur != nullptr) {
    if (cur->prefix_len_ != cur->check_prefix(key + depth, key_len - depth)) {
      /* prefix mismatch */
      return nullptr;
    }
    if (cur->prefix_len_ == key_len - depth) {
      /* exact match */
      if (!cur->is_leaf()) {
        return nullptr;
      }
      if (!std::strcmp(key, static_cast<leaf_node<T>*>(cur)->key_)) {
        /* keys do not compare equal */
        return nullptr;
      }
      return static_cast<leaf_node<T>*>(cur)->value_;
    }
    child = static_cast<inner_node<T>*>(cur)->find_child(key[depth + cur->prefix_len_]);
    depth += (cur->prefix_len_ + 1);
    cur = child != nullptr ? *child : nullptr;
  }
  return nullptr;
}

template <class T> T *art<T>::set(const char *key, T *value) {
  int key_len = std::strlen(key) + 1, depth = 0, prefix_match_len;
  if (root_ == nullptr) {
    root_ = new leaf_node<T>(key, value);
    root_->prefix_len_ = key_len;
    return nullptr;
  }

  node<T> **cur = &root_, **child;
  inner_node<T> **cur_inner;
  char child_partial_key;
  bool is_prefix_match;

  while (true) {
    if ((**cur).is_leaf()) {
      auto cur_leaf = reinterpret_cast<leaf_node<T>**>(cur);
      int key_match_len = 0;
      for (; key[key_match_len] == (**cur_leaf).key_[key_match_len]; ++key_match_len) {}
      if (key_match_len < depth) {
        /*
        return nullptr;
      }
      if (key_len == depth + (**cur).prefix_len_ && !std::strcmp(key, (**cur).key_)) {
        /* exact match:
         * => "replace"
         * => replace value of current node.
         * => return old value to caller to handle.
         *        _                             _
         *        |                             |
         *       (aa)                          (aa)
         *    a /    \ b     +[aaaaa,v3]    a /    \ b
         *     /      \      ==========>     /      \
         * *(aa)->v1  ()->v2             *(aa)->v3  ()->v2
         *
         */

        T *old_value = (**cur_leaf)->value_;
        (**cur_leaf).value_ = value;
        return old_value;
      }
      
      /* prefix mismatch:
       * => new parent node with common prefix and no associated value.
       * => new node with value to insert.
       * => current and new node become children of new parent node.
       *
       *        |                        |
       *      *(aa)                    +(a)   
       *    a /    \ b     +[ab,v3]  a /   \ b
       *     /      \      =======>   /     \
       *  (aa)->v1  ()->v2          *()    +()->v3
       *                          a /   \ b
       *                           /     \
       *                        (aa)->v1 ()->v2
       *                        /|\      /|\
       */

      int prefix_match_len = 0;
      for (; key[depth + prefix_match_len] == (**cur_leaf).key_[depth + prefix_match_len]; ++prefix_match_len) {}

      auto new_parent = new node_4<T>();
      new_parent->prefix_len_ = prefix_match_len;
      new_parent->set_child(key[depth + prefix_match_len], *cur);

      (**cur).prefix_len_ -= prefix_match_len + 1;

      auto new_node = new leaf_node<T>(key, value);
      new_node->prefix_len_ = key_len - depth - prefix_match_len - 1;
      new_parent->set_child(key[depth + prefix_match_len], new_node);

      *cur = new_parent;
      return nullptr;
    }

    /* must be inner node */
    cur_inner = reinterpret_cast<inner_node<T>**>(cur);
    child_partial_key = key[depth + (**cur).prefix_len_];
    child = (**cur_inner).find_child(child_partial_key);

    if (child == nullptr) {
      /*
       * no child associated with the next partial key.
       * => create new node with value to insert.
       * => new node becomes current node's child.
       *
       *      *(aa)              *(aa)   
       *    a /     +[aab,v2]  a /    \ b
       *     /      ========>   /      \
       *   (a)->v1            (a)->v1 +()->v2
       */

      if ((**cur_inner).is_full()) {
        *cur_inner = (**cur_inner).grow();
      }

      auto new_node = new leaf_node<T>(key, value);
      new_node->prefix_len_ = key_len - depth - (**cur).prefix_len_ - 1;
      (**cur_inner).set_child(child_partial_key, new_node);
      return nullptr;
    }

    /* propagate down and repeat:
     *
     *     *(aa)->Ø                   (aa)->Ø
     *   a /    \ b    +[aaba,v3]  a /    \ b     repeat
     *    /      \     =========>   /      \     ========>  ...
     *  (a)->v1  ()->v2           (a)->v1 *()->v2
     */

    depth += (**cur).prefix_len_ + 1;
    cur = child;
  }
}

template <class T> T *art<T>::del(const char *key) {
  int depth = 0, key_len = std::strlen(key) + 1;
  if (root_ == nullptr) {
    return nullptr;
  }

  /* pointer to parent, current and child node */
  node<T> **cur = &root_;
  inner_node<T> **par = nullptr, **cur_inner;

  /* partial key of current and child node */
  char cur_partial_key;

  while (cur != nullptr) {
    if ((**cur).prefix_len_ !=
        (**cur).check_prefix(key + depth, key_len - depth)) {
      /* prefix mismatch => key doesn't exist */

      return nullptr;
    }

    if (key_len == depth + (**cur).prefix_len_) {
      /* exact match */
      if (!(**cur).is_leaf()) {
        return nullptr;
      }
      auto value = static_cast<leaf_node<T>*>(*cur)->value_;
      auto n_siblings = par != nullptr ? (**par).n_children() - 1 : 0;

      if (n_siblings == 0) {
        /*
         * => must be root node
         * => delete root node
         *
         *     |                 |
         *    (aa)->v1          (aa)->v1
         *     | a     -[aaaaa]
         *     |       =======>
         *   *(aa)->v2
         */

        if ((**cur).prefix_ != nullptr) {
          delete[](**cur).prefix_;
        }
        delete (*cur);
        *cur = nullptr;

      } else if (n_siblings == 1) {
        /* => delete leaf node
         * => replace parent with sibling
         *
         *        |a                         |a
         *        |                          |
         *       (aa)        -"aaaaabaa"     |
         *    a /    \ b     ==========>    /
         *     /      \                    /
         *  (aa)->v1 *()->v2             (aaaaa)->v1
         *  /|\                            /|\
         */

        /* find sibling */
        auto sibling_partial_key = (**par).next_partial_key(0);
        if (sibling_partial_key == cur_partial_key) {
          sibling_partial_key = (**par).next_partial_key(cur_partial_key + 1);
        }
        auto sibling = *(**par).find_child(sibling_partial_key);

        auto old_prefix = sibling->prefix_;
        auto old_prefix_len = sibling->prefix_len_;

        sibling->prefix_ = new char[(**par).prefix_len_ + 1 + old_prefix_len];
        sibling->prefix_len_ = (**par).prefix_len_ + 1 + old_prefix_len;
        std::copy((**par).prefix_, (**par).prefix_ + (**par).prefix_len_,
                  sibling->prefix_);
        sibling->prefix_[(**par).prefix_len_] = sibling_partial_key;
        std::copy(old_prefix, old_prefix + old_prefix_len,
                  sibling->prefix_ + (**par).prefix_len_ + 1);
        if (old_prefix != nullptr) {
          delete[] old_prefix;
        }
        if ((**cur).prefix_ != nullptr) {
          delete[](**cur).prefix_;
        }
        delete (*cur);
        if ((**par).prefix_ != nullptr) {
          delete[](**par).prefix_;
        }
        delete (*par);

        /* this looks crazy, but I know what I'm doing */
        *par = static_cast<inner_node<T>*>(sibling);

      } else /* if (n_siblings > 1) */ {
        /* => delete leaf node
         *
         *        |a                         |a
         *        |                          |
         *       (aa)        -"aaaaabaa"    (aa)   
         *    a / |  \ b     ==========> a / |
         *     /  |   \                   /  |
         *           *()->v1
         */

        if ((**cur).prefix_ != nullptr) {
          delete[](**cur).prefix_;
        }
        delete (*cur);
        (**par).del_child(cur_partial_key);
        if ((**par).is_underfull()) {
          *par = (**par).shrink();
        }
      }

      return value;
    }

    /* propagate down and repeat */
    cur_partial_key = key[depth + (**cur).prefix_len_];
    depth += (**cur).prefix_len_ + 1;
    par = reinterpret_cast<inner_node<T>**>(cur);
    cur = (**par).find_child(cur_partial_key);
  }
  return nullptr;
}

template <class T> tree_it<T> art<T>::begin() {
  return tree_it<T>::min(this->root_);
}

template <class T> tree_it<T> art<T>::begin(const char *key) {
  return tree_it<T>::greater_equal(this->root_, key);
}

template <class T> tree_it<T> art<T>::end() { return tree_it<T>(); }

} // namespace art

#endif
