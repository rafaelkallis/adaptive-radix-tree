#include <art.hpp>
#include <string>
#include <random>

int main() {
  art::art<int> m;
  int v = 1;
  std::random_device rd;
  std::mt19937_64 g(rd());
  for (uint32_t i = 0; i < 1000000; ++i) {
    m.set(std::to_string(i), &v);
  }
}
