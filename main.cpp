#include "cache.h"
#include <cassert>
#include <random>
#include <thread>

int main(int argc, char *argv[]) {
  size_t numThreads = 100;
  size_t numIters = 1000000;
  using TestCache = SimpleCache<int, int>;
  TestCache cache(300);
  auto op = [numIters, &cache]() {
    std::mt19937_64 randomGenerator;
    for (size_t i = 0; i < numIters; i++) {
      int key = randomGenerator();
      TestCache::FloatingRecord record = cache.Find(key);
      if (record) {
        assert(record->value == key);
      }

      TestCache::FloatingRecord replacement =
          std::make_shared<TestCache::Record>();
      replacement->key = key;
      replacement->value = key;
      cache.Store(replacement);
    }
  };

  // Cache is simply fn f(x) = x;
  // Launch 4 threads, continuously load/store arbitrary (x, x) pairs.
  std::vector<std::thread> workers;
  for (size_t t = 0; t < numThreads; t++) {
    workers.emplace_back(op);
  }

  for (size_t t = 0; t < numThreads; t++) {
    workers[t].join();
  }

  return 0;
}
