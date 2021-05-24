/**
 * @file query microbenchmarks
 * @author Rafael Kallis <rk@rafaelkallis.com>
 */

#include "art.hpp"
#include "picobench/picobench.hpp"
#include <functional>
#include <map>
#include <queue>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <cstring>

using picobench::state;
using std::hash;
using std::map;
using std::mt19937_64;
using std::string;
using std::to_string;
using std::unordered_map;

PICOBENCH_SUITE("query sparse uniform base 64 keys");

std::string to_base64(const std::string& string_);

static void art_q_s_u(state &s) {
  art::art<int*> m;
  hash<uint32_t> h;
  int v = 1;
  int *v_ptr = &v;
  mt19937_64 rng1(0);
  for (auto i __attribute__((unused)) : s) {
    m.set(to_base64(to_string(h(rng1()))).c_str(), v_ptr);
  }
  mt19937_64 rng2(0);
  for (auto i __attribute__((unused)) : s) {
    v_ptr = m.get(to_base64(to_string(h(rng2()))).c_str());
  }
}
PICOBENCH(art_q_s_u)
/* .iterations({4000000}) */
;

static void red_black_q_s_u(state &s) {
  map<string, int> m;
  hash<uint32_t> h;
  int v = 1;
  mt19937_64 rng1(0);
  for (auto i __attribute__((unused)) : s) {
    m[to_base64(to_string(h(rng1())))] = v;
  }
  mt19937_64 rng2(0);
  for (auto i __attribute__((unused)) : s) {
    v = m[to_base64(to_string(h(rng2())))];
  }
}
PICOBENCH(red_black_q_s_u)
/* .iterations({4000000}) */
;

static void hashmap_q_s_u(state &s) {
  unordered_map<string, int> m;
  hash<uint32_t> h;
  int v = 1;
  mt19937_64 rng1(0);
  for (auto i __attribute__((unused)) : s) {
    m[to_base64(to_string(h(rng1())))] = v;
  }
  mt19937_64 rng2(0);
  for (auto i __attribute__((unused)) : s) {
    v = m[to_base64(to_string(h(rng2())))];
  }
}
PICOBENCH(hashmap_q_s_u)
/* .iterations({4000000}) */
;

static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

std::string to_base64(const std::string& string_) {
  const char *bytes_to_encode = string_.c_str();
  int in_len = string_.length();
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] =
          ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] =
          ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for (i = 0; (i < 4); i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] =
        ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] =
        ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while ((i++ < 3))
      ret += '=';
  }

  return ret;
}
