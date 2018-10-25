#include "art.hpp"
#include "zipf.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <unordered_map>
#include <chrono>
#include <thread>

using std::string;

void art_bench() {
  std::ifstream file("dataset.txt");
  std::unordered_map<uint32_t, string> dataset;
  uint32_t n = 0;
  string line;
  while (std::getline(file, line)) {
    dataset[n++] = line;
  }
  file.close();

  fast_zipf rng(n);
  art::art<int> m;
  /* std::map<art::key_type, int*> m; */
  /* std::unordered_map<art::key_type, int*> m; */
  int v = 1;
  std::mt19937_64 g(0);
  for (uint32_t i = 0; i < 1000000; ++i) {
    auto k = dataset[rng()];
    m.set(k.c_str(), &v);
    /* m[dataset[rng()]] = &v; */
  }
}

/* void art_sparse_uniform() { */
/*   art::art<int> m; */
/*   int n = 1000; */
/*   int v = 1; */
/*   std::mt19937_64 rng1(0); */
/*   std::string k; */
/*   for (int i = 0; i < n; ++i) { */
/*     k = std::to_string(rng1()); */
/*     m.set(k.c_str(), &v); */
/*   } */
  
/*   std::mt19937_64 rng2(0); */
/*   for (int i = 0; i < n; ++i) { */
/*     k = std::to_string(rng2()); */
/*     m.get(k.c_str()); */
/*   } */
  
/*   std::mt19937_64 rng3(0); */
/*   for (int i = 0; i < n; ++i) { */
/*     k = std::to_string(rng3()); */
/*     m.del(k.c_str()); */
/*   } */
/* } */

/* std::string pad(const std::string &s, char ch, int n) { */
/*   if (s.length() >= n) { */
/*     return s; */
/*   } */
/*   std::string new_s(n, ch); */
/*   for (int i = s.length() - 1; i > -1; --i) { */
/*     new_s[n - i - 1] = s[i]; */
/*   } */
/*   return new_s; */
/* } */

/* void art_compressions_dense_insert() { */
/*   art::art<int> m; */
/*   int v = 1; */
/*   std::string k; */
/*   int i; */
/*   for (i = 0; i < 10000000; ++i) { */
/*     if (i % 1000000 == 0) { */
/*       std::cout << i << " " << m.n_compress << std::endl; */
/*     } */
/*     k = pad(std::to_string(i), '0', 7); */
/*     m.set(k.c_str(), &v); */
/*   } */
/*   std::cout << i << " " << m.n_compress << std::endl; */
/* } */

/* void art_compressions_dense_delete() { */
/*   art::art<int> m; */
/*   int v = 1; */
/*   std::string k; */
/*   int i; */
/*   for (i = 0; i < 10000000; ++i) { */
/*     k = pad(std::to_string(i), '0', 7); */
/*     m.set(k.c_str(), &v); */
/*   } */
/*   m.n_compress = 0; */
/*   for (i = 0; i < 10000000; ++i) { */
/*     if (i % 1000000 == 0) { */
/*       std::cout << i << " " << m.n_compress << std::endl; */
/*     } */
/*     k = pad(std::to_string(i), '0', 7); */
/*     m.del(k.c_str()); */
/*   } */
/*   std::cout << i << " " << m.n_compress << std::endl; */
/* } */

/* void art_compressions_paths_insert() { */
/*   art::art<int> m; */
/*   int v = 1; */
/*   auto file = std::ifstream("dataset.txt"); */
/*   std::string line; */
/*   int i = 0; */
/*   while (std::getline(file, line)) { */
/*     if (i % 1000000 == 0) { */
/*       std::cout << i << " " << m.n_compress << std::endl; */
/*     } */
/*     m.set(line.c_str(), line.length(), &v); */
/*     ++i; */
/*   } */
/*   file.close(); */
/*   std::cout << i << " " << m.n_compress << std::endl; */
/* } */

/* void art_compressions_paths_delete() { */
/*   art::art<int> m; */
/*   int v = 1; */
/*   auto file = std::ifstream("dataset.txt"); */
/*   std::string line; */
/*   while (std::getline(file, line)) { */
/*     m.set(line.c_str(), line.length(), &v); */
/*   } */
/*   file.close(); */
/*   file = std::ifstream("dataset.txt"); */
/*   m.n_compress = 0; */
/*   int i = 0; */
/*   while (std::getline(file, line)) { */
/*     if (i % 1000000 == 0) { */
/*       std::cout << i << " " << m.n_compress << std::endl; */
/*     } */
/*     m.del(line.c_str(), line.length()); */
/*     ++i; */
/*   } */
/*   file.close(); */
/*   std::cout << i << " " << m.n_compress << std::endl; */
/* } */

/* void art_compressions_sparse_insert() { */
/*   art::art<int> m; */
/*   int v = 1; */
/*   std::mt19937_64 rng1(0); */
/*   std::string k; */
/*   int i; */
/*   for (i = 0; i < 10000000; ++i) { */
/*     if (i % 1000000 == 0) { */
/*       std::cout << i << " " << m.n_compress << std::endl; */
/*     } */
/*     k = std::to_string(rng1()); */
/*     m.set(k.c_str(), &v); */
/*   } */
/*   std::cout << i << " " << m.n_compress << std::endl; */
/* } */

/* void art_compressions_sparse_delete() { */
/*   art::art<int> m; */
/*   int v = 1; */
/*   std::mt19937_64 rng1(0); */
/*   std::string k; */
/*   for (int i = 0; i < 10000000; ++i) { */
/*     k = std::to_string(rng1()); */
/*     m.set(k.c_str(), &v); */
/*   } */
/*   m.n_compress = 0; */
/*   std::mt19937_64 rng2(0); */
/*   int i = 0; */
/*   for (; i < 10000000; ++i) { */
/*     if (i % 1000000 == 0) { */
/*       std::cout << i << " " << m.n_compress << std::endl; */
/*     } */
/*     k = std::to_string(rng2()); */
/*     m.del(k.c_str()); */
/*   } */
/*   std::cout << i << " " << m.n_compress << std::endl; */
/* } */

void casual_stress_test(int n) {
  art::art<int> m;
  int v = 1;
  std::mt19937_64 rng1(0);
  std::string k;
  int i;
  for (i = 0; i < n; ++i) {
    k = std::to_string(rng1());
    m.set(k.c_str(), &v);
    if ((i % (1<<20)) == 0) {
      std::cout << i << std::endl;
    }
  }
  std::cout << i << std::endl;
}

int main() {
  /* std::cout << "sparse insert" << std::endl; */
  /* art_compressions_sparse_insert(); */
  /* std::cout << "sparse delete" << std::endl; */
  /* art_compressions_sparse_delete(); */
  /* std::cout << "paths insert" << std::endl; */
  /* art_compressions_paths_insert(); */
  /* std::cout << "paths delete" << std::endl; */
  /* art_compressions_paths_delete(); */
  /* std::cout << "dense insert" << std::endl; */
  /* art_compressions_dense_insert(); */
  /* std::cout << "dense delete" << std::endl; */
  /* art_compressions_dense_delete(); */

  /* casual_stress_test(16 * 1000 * 1000); */
  int int0 = 0;
  int int1 = 1;
  int int2 = 2;
  int int3 = 4;
  int int4 = 5;
  int int5 = 5;
  int int6 = 6;

  art::art<int> m;

  m.set("aa", &int0);
  m.set("aaaa", &int1);
  m.set("aaaaaaa", &int2);
  m.set("aaaaaaaaaa", &int3);
  m.set("aaaaaaaba", &int4);
  m.set("aaaabaa", &int5);
  m.set("aaaabaaaaa", &int6);

  /* The above statements construct the following tree:
   *
   *          (aa)
   *   $_____/ |a
   *   /       |
   *  ()->0   (a)
   *   $_____/ |a\____________b       
   *   /       |              \
   *  ()->1   (aa)            (aa)
   *   $_____/ |a\___b         |$\____a
   *   /       |     \         |      \
   *  ()->2 (aa$)->3 (a$)->4 ()->5 (aa$)->6
   *
   */

  auto it = m.begin();
  auto it_end = m.end();
  for (int i = 0; it != it_end; ++i, ++it) {
    std::cout << i << ": " << **it << std::endl;
  }
  return 0;
}
