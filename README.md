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

## Benchmark results (16M keys, `art::art` vs `std::map` vs `std::unordered_map`)
```
delete:
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
      art_delete_sparse * |16000000 | 30773.240 |    1923 |      - |   519932.3
  red_black_delete_sparse |16000000 | 32789.134 |    2049 |  1.066 |   487966.5
    hashmap_delete_sparse |16000000 | 10224.530 |     639 |  0.332 |  1564864.1
===============================================================================

insert:
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
      art_insert_sparse * |16000000 | 17159.731 |    1072 |      - |   932415.5
  red_black_insert_sparse |16000000 | 28311.689 |    1769 |  1.650 |   565137.6
    hashmap_insert_sparse |16000000 | 13572.377 |     848 |  0.791 |  1178865.0
===============================================================================

mixed:
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
       art_mixed_sparse * |16000000 | 13814.544 |     863 |      - |  1158199.6
   red_black_mixed_sparse |16000000 | 25327.647 |    1582 |  1.833 |   631720.7
     hashmap_mixed_sparse |16000000 | 12032.045 |     752 |  0.871 |  1329782.3
===============================================================================

query sparse uniform:
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
              art_q_s_u * |16000000 | 12871.849 |     804 |      - |  1243022.6
          red_black_q_s_u |16000000 | 25548.772 |    1596 |  1.985 |   626253.2
            hashmap_q_s_u |16000000 |  6862.054 |     428 |  0.533 |  2331663.4
===============================================================================

query sparse uniform base 64 keys:
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
              art_q_s_u * |16000000 | 16303.116 |    1018 |      - |   981407.5
          red_black_q_s_u |16000000 | 27618.685 |    1726 |  1.694 |   579317.9
            hashmap_q_s_u |16000000 |  8005.581 |     500 |  0.491 |  1998605.8
===============================================================================

query sparse zipf:
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
              art_q_s_z * |16000000 |  6785.841 |     424 |      - |  2357850.7
          red_black_q_s_z |16000000 | 10559.469 |     659 |  1.556 |  1515227.7
            hashmap_q_s_z |16000000 |  6896.744 |     431 |  1.016 |  2319935.2
===============================================================================
```

## References

* [The Adaptive Radix Tree: ARTful Indexing for Main-Memory Databases](http://www-db.in.tum.de/~leis/papers/ART.pdf)
* [The Adaptive Radix Tree](http://rafaelkallis.com/static/media/the_adaptive_radix_tree_rafael_kallis.23779b20.pdf)
