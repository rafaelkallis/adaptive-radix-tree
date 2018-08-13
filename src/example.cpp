#include "art.hpp"
#include "zipf.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <random>
#include <string>
#include <unordered_map>

void art_bench() {
  std::ifstream file("dataset.txt");
  if (!file) {
    return;
  }
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

int main() {
  return 0;
}
