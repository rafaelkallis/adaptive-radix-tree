# Adaptive Radix Tree

The goal of this project is to study and implement the Adaptive Radix Tree (ART), 
as proposed by Leis et al. ART, which is a trie based data structure, achieves 
its performance, and space efficiency, by compressing the tree both vertically, 
i.e., if a node has no siblings it is merged with its parent, and horizontally, 
i.e., uses an array which grows as the number of children increases. Vertical 
compression reduces the tree height and horizontal compression decreases a node’s size.

## Usage

```cpp
#include <art.hpp>

using namespace art;

int main() {
  art<int> m;

  int v = 2;
  // set k to v
  m.set("k", &v);

  // get value of k
  int * v_ptr = m.get("k");

  // delete k
  m.del("k");

  return 0;
}
```

## Contributing

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
query:
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
              art_q_s_u * |  100000 |    28.607 |     286 |      - |  3495600.0
          red_black_q_s_u |  100000 |    42.799 |     427 |  1.496 |  2336486.7
            hashmap_q_s_u |  100000 |    21.123 |     211 |  0.738 |  4734209.2
===============================================================================

query zipf:
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
              art_q_s_z * |  100000 |    33.452 |     334 |      - |  2989315.3
          red_black_q_s_z |  100000 |    45.925 |     459 |  1.373 |  2177458.6
            hashmap_q_s_z |  100000 |    33.832 |     338 |  1.011 |  2955786.5
===============================================================================

delete:
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
      art_delete_sparse * |  100000 |    83.504 |     835 |      - |  1197553.6
  red_black_delete_sparse |  100000 |    62.600 |     625 |  0.750 |  1597444.9
    hashmap_delete_sparse |  100000 |    35.515 |     355 |  0.425 |  2815721.2
===============================================================================

insert:
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
      art_insert_sparse * |  100000 |    48.764 |     487 |      - |  2050703.1
  red_black_insert_sparse |  100000 |    52.430 |     524 |  1.075 |  1907320.1
    hashmap_insert_sparse |  100000 |    32.163 |     321 |  0.660 |  3109120.5
===============================================================================

mixed:
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
       art_mixed_sparse * |  100000 |    55.786 |     557 |      - |  1792564.1
   red_black_mixed_sparse |  100000 |    89.380 |     893 |  1.602 |  1118821.9
     hashmap_mixed_sparse |  100000 |    53.493 |     534 |  0.959 |  1869392.0
===============================================================================
```

## References

* [The Adaptive Radix Tree: ARTful Indexing for Main-Memory Databases](http://www-db.in.tum.de/~leis/papers/ART.pdf)
* [The Adaptive Radix Tree](http://rafaelkallis.com/static/media/the_adaptive_radix_tree_rafael_kallis.23779b20.pdf)
