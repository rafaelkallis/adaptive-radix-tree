# Adaptive Radix Trie

## Usage

```cpp
#include <art.hpp>

using namespace art;

art tree;
```

## Test

```cpp
# build test binaries
make [release]

# run tests
make test

# run benchmarks
make bench
```

## Benchmark results

```
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
      art_insert_sparse * | 1048576 |   682.231 |     650 |      - |  1536981.4
    std_map_insert_sparse | 1048576 |  1191.772 |    1136 |  1.747 |   879845.8
        art_delete_sparse | 1048576 |   549.585 |     524 |  0.806 |  1907940.0
    std_map_delete_sparse | 1048576 |  1157.661 |    1104 |  1.697 |   905771.6
===============================================================================
```

## References

* [The Adaptive Radix Tree: ARTful Indexing for Main-Memory Databases](http://www-db.in.tum.de/~leis/papers/ART.pdf)
