/**
 * @file pair
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#ifndef ART_PAIR_HPP
#define ART_PAIR_HPP

#include "constants.hpp"
#include <string>

namespace art {

using std::string;

template <class T> class pair {
public:
  pair() = default;
  pair(const string &key, const T &value);
  ~pair();
  pair(const pair<T> &other) = default;
  pair(pair<T> &&other) noexcept = default;

  pair<T> &operator=(const pair<T> &other) = default;
  pair<T> &operator=(pair<T> &&other) noexcept = default;

  key_type key_;
  T value_;
};

template <class T>
pair<T>::pair(const string &key, const T &value)
    : key_(new partial_key_type[key.length()]), value_(value) {
  std::memcpy(key_, key.begin(), key.length());
}

template <class T> pair<T>::~pair() {
  if (key_ != nullptr) {
    delete key_;
  }
}

} // namespace art

#endif
