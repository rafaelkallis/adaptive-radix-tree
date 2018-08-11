# Adaptive Radix Trie

## Usage

```cpp
#include <art.hpp>

using namespace art;

art<int> tree;
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

#### Insertion
```
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
      art_insert_sparse * | 1048576 |   662.458 |     631 |      - |  1582856.8
  red_black_insert_sparse | 1048576 |  1047.248 |     998 |  1.581 |  1001268.3
    hashmap_insert_sparse | 1048576 |   651.702 |     621 |  0.984 |  1608979.6
===============================================================================
```

#### Removal
```
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
      art_delete_sparse * | 1048576 |   556.471 |     530 |      - |  1884331.6
  red_black_delete_sparse | 1048576 |  1069.961 |    1020 |  1.923 |   980013.0
    hashmap_delete_sparse | 1048576 |   392.914 |     374 |  0.706 |  2668716.3
===============================================================================
```

## References

* [The Adaptive Radix Tree: ARTful Indexing for Main-Memory Databases](http://www-db.in.tum.de/~leis/papers/ART.pdf)
