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

namespace art {

template <class T> class art {
public:
  art() = default;
  art(const art<T> &other);
  art(art<T> &&other) noexcept;
  ~art();

  art<T> &operator=(const art<T> &other);
  art<T> &operator=(art<T> &&other);

  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using iterator = tree_it<T>;
  using const_iterator = tree_it<const T>;

  /**
   * Finds the value associated with the given key.
   *
   * @param key - The key to find.
   * @return the value associated with the key or a nullptr.
   */
  T get(const char *key) const;

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
  T set(const char *key, T value);

  /**
   * Deletes the given key and returns it's associated value.
   * The associated value is returned,
   * since the method consumer is the resource owner.
   * If no value is associated with the given key, nullptr is returned.
   *
   * @param key - The key to delete.
   * @return the values assciated with they key or a nullptr otherwise.
   */
  T del(const char *key);

  /**
   * Forward iterator that traverses the tree in lexicographic order.
   */
  tree_it<T> begin() const;
  tree_it<T> cbegin() const;

  /**
   * Forward iterator that traverses the tree in lexicographic order starting
   * from the provided key.
   */
  tree_it<T> begin(const char *key) const;
  tree_it<T> cbegin(const char *key) const;

  /**
   * Iterator to the end of the lexicographic order.
   */
  tree_it<T> end() const;
  tree_it<T> cend() const;

private:
  node<T> *root_ = nullptr;

  const art<T> &as_const() const;
};

template <class T> 
art<T>::art(const art<T> &other) {
  // TODO traverse internal structure
  for (auto it = other.begin(), it_end = other.end(); it != it_end; ++it) {
    set(it.key().c_str(), *it);
  }
}

template <class T> 
art<T>::art(art<T> &&other) noexcept : root_(other.root_) {
  other.root_ = nullptr;
}

template <class T> 
art<T>::~art() {
  if (root_ == nullptr) {
    return;
  }
  std::stack<const node<T> *> node_stack;
  node_stack.push(root_);
  while (!node_stack.empty()) {
    const node<T> *cur = node_stack.top();
    node_stack.pop();
    if (!cur->is_leaf()) {
      auto it = cur->as_inner()->begin();
      auto it_end = cur->as_inner()->end();
      for (; it != it_end; ++it) {
        const node<T> *child_node = it.get_child_node();
        node_stack.push(child_node);
      }
    }
    delete cur;
  }
}

template <class T> 
art<T>& art<T>::operator=(const art<T> &other) {
  if (this != &other) {
    // TODO traverse internal structure
    for (auto it = other.begin(), it_end = other.end(); it != it_end; ++it) {
      set(it.key().c_str(), *it);
    }
  }
  return *this;
}

template <class T> 
art<T>& art<T>::operator=(art<T> &&other) {
  if (this != &other) {
    root_ = other.root_;
    other.root_ = nullptr;
  }
  return *this;
}

template <class T> 
T art<T>::get(const char *key) const {
  const node<T> *cur = root_;
  int depth = 0, key_len = std::strlen(key) + 1;
  while (cur != nullptr) {
    if (cur->prefix_len_ != cur->check_prefix(key + depth, key_len - depth)) {
      /* prefix mismatch */
      return T{};
    }
    if (cur->prefix_len_ == key_len - depth) {
      /* exact match */
      return cur->is_leaf() ? cur->as_leaf()->value_ : T{};
    }
    auto child = cur->as_inner()->find_child(key[depth + cur->prefix_len_]);
    depth += (cur->prefix_len_ + 1);
    cur = child != nullptr ? *child : nullptr;
  }
  return T{};
}

template <class T> 
T art<T>::set(const char *key, T value) {
  int key_len = std::strlen(key) + 1, depth = 0, prefix_match_len;
  if (root_ == nullptr) {
    root_ = new leaf_node<T>(key, key_len, value);
    return T{};
  }

  node<T> **cur = &root_, **child;
  inner_node<T> **cur_inner;
  char child_partial_key;
  bool is_prefix_match;

  while (true) {
    /* number of bytes of the current node's prefix that match the key */
    prefix_match_len = (**cur).check_prefix(key + depth, key_len - depth);

    /* true if the current node's prefix matches with a part of the key */
    is_prefix_match = (std::min<int>((**cur).prefix_len_, key_len - depth)) ==
                      prefix_match_len;

    if (is_prefix_match && (**cur).prefix_len_ == key_len - depth) {
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

      /* cur must be a leaf */
      auto cur_leaf = static_cast<leaf_node<T>*>(*cur);
      T old_value = cur_leaf->value_;
      cur_leaf->value_ = value;
      return old_value;
    }

    if (!is_prefix_match) {
      /* prefix mismatch:
       * => new parent node with common prefix and no associated value.
       * => new node with value to insert.
       * => current and new node become children of new parent node.
       *
       *        |                        |
       *      *(aa)                    +(a)->Ø
       *    a /    \ b     +[ab,v3]  a /   \ b
       *     /      \      =======>   /     \
       *  (aa)->v1  ()->v2          *()->Ø +()->v3
       *                          a /   \ b
       *                           /     \
       *                        (aa)->v1 ()->v2
       *                        /|\      /|\
       */

      auto new_parent = new node_4<T>();
      new_parent->prefix_ = new char[prefix_match_len];
      std::copy_n((**cur).prefix_, prefix_match_len, new_parent->prefix_);
      new_parent->prefix_len_ = prefix_match_len;
      new_parent->set_child((**cur).prefix_[prefix_match_len], *cur);

      // TODO(rafaelkallis): shrink?
      /* memmove((**cur).prefix_, (**cur).prefix_ + prefix_match_len + 1, */
      /*         (**cur).prefix_len_ - prefix_match_len - 1); */
      /* (**cur).prefix_len_ -= prefix_match_len + 1; */

      auto old_prefix = (**cur).prefix_;
      auto old_prefix_len = (**cur).prefix_len_;
      (**cur).prefix_ = new char[old_prefix_len - prefix_match_len - 1];
      (**cur).prefix_len_ = old_prefix_len - prefix_match_len - 1;
      std::copy_n(old_prefix + prefix_match_len + 1, (**cur).prefix_len_, (**cur).prefix_);
      delete old_prefix;

      auto new_node = new leaf_node<T>(
        key + depth + prefix_match_len + 1, 
        key_len - depth - prefix_match_len - 1, 
        value
      );
      new_parent->set_child(key[depth + prefix_match_len], new_node);

      *cur = new_parent;
      return T{};
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
       *      *(aa)->Ø              *(aa)->Ø
       *    a /        +[aab,v2]  a /    \ b
       *     /         ========>   /      \
       *   (a)->v1               (a)->v1 +()->v2
       */

      if ((**cur_inner).is_full()) {
        *cur_inner = (**cur_inner).grow();
      }

      auto new_node = new leaf_node<T>(
        key + depth + (**cur).prefix_len_ + 1,
        key_len - depth - (**cur).prefix_len_ - 1,
        value
      );
      (**cur_inner).set_child(child_partial_key, new_node);
      return T{};
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

template <class T> 
T art<T>::del(const char *key) {
  int depth = 0, key_len = std::strlen(key) + 1;

  if (root_ == nullptr) {
    return T{};
  }

  /* pointer to parent, current and child node */
  node<T> **cur = &root_;
  inner_node<T> **par = nullptr;

  /* partial key of current and child node */
  char cur_partial_key = 0;

  while (cur != nullptr) {
    if ((**cur).prefix_len_ !=
        (**cur).check_prefix(key + depth, key_len - depth)) {
      /* prefix mismatch => key doesn't exist */

      return T{};
    }

    if (key_len == depth + (**cur).prefix_len_) {
      /* exact match */
      if (!(**cur).is_leaf()) {
        return T{};
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

        assert(*cur == root_);
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
        std::copy_n((**par).prefix_, (**par).prefix_len_, sibling->prefix_);
        sibling->prefix_[(**par).prefix_len_] = sibling_partial_key;
        std::copy_n(old_prefix, old_prefix_len, sibling->prefix_ + (**par).prefix_len_ + 1);
        delete [] old_prefix;
        delete (*cur);
        *cur = nullptr;
        delete (*par);
        *par = nullptr;

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

        assert(n_siblings > 1);
        node<T> *child_to_delete = (**par).del_child(cur_partial_key);
        assert(child_to_delete->is_leaf());
        delete child_to_delete;
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
  return T{};
}

template <class T> 
tree_it<T> art<T>::begin() const {
  return tree_it<T>::min(this->root_);
}

template <class T> 
tree_it<T> art<T>::cbegin() const {
  return begin();
}

template <class T> tree_it<T> art<T>::begin(const char *key) const {
  return tree_it<T>::greater_equal(this->root_, key);
}

template <class T> tree_it<T> art<T>::cbegin(const char *key) const {
  return begin(key);
}

template <class T> tree_it<T> art<T>::end() const { 
  return tree_it<T>(); 
}

template <class T> tree_it<T> art<T>::cend() const { 
  return tree_it<T>(); 
}

template <class T> const art<T>& art<T>::as_const() const { 
  return static_cast<const art<T> &>(*this); 
}

} // namespace art

#endif
