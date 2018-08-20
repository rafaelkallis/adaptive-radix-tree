#include "art.hpp"
#include "zipf.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <unordered_map>

void art_bench() {
  std::ifstream file("dataset.txt");
  std::unordered_map<uint32_t, std::string> dataset;
  uint32_t n = 0;
  std::string line;
  while (std::getline(file, line)) {
    dataset[n++] = line;
  }
  file.close();

  fast_zipf rng(n, 1.0, 0);
  art::art<int> m;
  /* std::map<art::key_type, int*> m; */
  /* std::unordered_map<art::key_type, int*> m; */
  int v = 1;
  std::random_device rd;
  std::mt19937_64 g(rd());
  for (uint32_t i = 0; i < 1000000; ++i) {
    m.set(dataset[rng()], &v);
    /* m[dataset[rng()]] = &v; */
  }
}

/* void art_compressions() { */
/*   art::art<int> m; */
/*   int v = 1; */
/*   auto file = std::ifstream("dataset.txt"); */
/*   std::string line; */
/*   while (std::getline(file, line)) { */
/*     m.set(line, &v); */
/*   } */
/*   file.close(); */
/*   file = std::ifstream("dataset.txt"); */
/*   int i = 0; */
/*   while (std::getline(file, line)) { */
/*     if (i % 1000000 == 0) { */
/*       std::cout << i << " " << m.compression_count << std::endl; */
/*     } */
/*     m.del(line); */
/*     ++i; */
/*   } */
/*   file.close(); */
/*   std::cout << i << " " << m.compression_count << std::endl; */
/* } */

int main() {
  int v = 2;
  art::art<int> m;

  /* The above statements construct the following tree:
   *
   *                (aa)->0
   *                 |a
   *                 |
   *                (a)->1
   *             a /   \ b
   *              /     \
   *        2<-(aa)     (aa)->5
   *         a /  \ b    |a
   *          /    \     |
   *    3<-(aa) 4<-(a)  (aa)->6
   */

  m.set("aa", &v);
  m.set("aaaa", &v);
  m.set("aaaaaaa", &v);
  m.set("aaaaaaaaaa", &v);
  m.set("aaaaaaaaaaa", &v);
  m.set("aaaaaaaaaab", &v);
  m.set("aaaaaaaaaac", &v);
  m.set("aaaaaaaba", &v);
  m.set("aaaabaa", &v);
  m.set("aaaabaaaaa", &v);
  m.del("aaaabaaaaa");
  std::cout << 1 << std::endl;

  return 0;
}
