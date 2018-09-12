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
    m.set(k.c_str(), k.length(), &v);
    /* m[dataset[rng()]] = &v; */
  }
}

void art_sparse_uniform() {
  art::art<int> m;
  int n = 1000;
  int v = 1;
  std::mt19937_64 rng1(0);
  std::string k;
  for (int i = 0; i < n; ++i) {
    k = std::to_string(rng1());
    m.set(k.c_str(), k.length(), &v);
  }
  
  std::mt19937_64 rng2(0);
  for (int i = 0; i < n; ++i) {
    k = std::to_string(rng2());
    m.get(k.c_str(), k.length());
  }
  
  std::mt19937_64 rng3(0);
  for (int i = 0; i < n; ++i) {
    k = std::to_string(rng3());
    m.del(k.c_str(), k.length());
  }
}

std::string pad(const std::string &s, char ch, int n) {
  if (s.length() >= n) {
    return s;
  }
  std::string new_s(n, ch);
  for (int i = s.length() - 1; i > -1; --i) {
    new_s[n - i - 1] = s[i];
  }
  return new_s;
}

/* void art_compressions_dense() { */
/*   art::art<int> m; */
/*   int v = 1; */
/*   std::string k; */
/*   int i; */
/*   for (i = 0; i < 10000000; ++i) { */
/*     k = pad(std::to_string(i), '0', 7); */
/*     m.set(k.c_str(), k.length(), &v); */
/*   } */
/*   for (i = 0; i < 10000000; ++i) { */
/*     if (i % 1000000 == 0) { */
/*       std::cout << i << " " << m.n_compress << std::endl; */
/*     } */
/*     k = pad(std::to_string(i), '0', 7); */
/*     m.del(k.c_str(), k.length()); */
/*   } */
/*   std::cout << i << " " << m.n_compress << std::endl; */
/* } */

/* void art_compressions_paths() { */
/*   art::art<int> m; */
/*   int v = 1; */
/*   auto file = std::ifstream("dataset.txt"); */
/*   std::string line; */
/*   while (std::getline(file, line)) { */
/*     m.set(line.c_str(), line.length(), &v); */
/*   } */
/*   file.close(); */
/*   file = std::ifstream("dataset.txt"); */
/*   int i = 0; */
/*   while (std::getline(file, line)) { */
/*     if (i % 1000000 == 0) { */
/*       std::cout << i << " " << m.compression_count << std::endl; */
/*     } */
/*     m.del(line.c_str(), line.length()); */
/*     ++i; */
/*   } */
/*   file.close(); */
/*   std::cout << i << " " << m.compression_count << std::endl; */
/* } */

/* void art_compressions_sparse() { */
/*   art::art<int> m; */
/*   int v = 1; */
/*   std::mt19937_64 rng1(0); */
/*   std::string k; */
/*   for (int i = 0; i < 10000000; ++i) { */
/*     k = std::to_string(rng1()); */
/*     m.set(k.c_str(), k.length(), &v); */
/*   } */
/*   std::mt19937_64 rng2(0); */
/*   int i = 0; */
/*   for (; i < 10000000; ++i) { */
/*     if (i % 1000000 == 0) { */
/*       std::cout << i << " " << m.compression_count << std::endl; */
/*     } */
/*     k = std::to_string(rng2()); */
/*     m.del(k.c_str(), k.length()); */
/*   } */
/*   std::cout << i << " " << m.compression_count << std::endl; */
/* } */

int main() {
  /* art_sparse_uniform(); */
  /* art_compressions_sparse(); */
  /* art_compressions_dense(); */
  return 0;
}
