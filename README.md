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
  art<std::shared_ptr<MyResource>> m;

  // set k
  m.set("k", std::make_shared(new MyResource()));

  // get k
  std::shared_ptr<MyResource> ptr = m.get("k");

  // delete k
  m.del("k");

  return 0;
}
```

## Contributing

```cpp
# clone repository with submodules
git clone --recurse-submodules https://github.com/rafaelkallis/adaptive-radix-tree

# build test binaries
make [release]

# run tests
make test

# run benchmarks
make bench

# run memory benchmarks (requires valgrind)
make bench-mem
```

## Benchmark results 
(16M keys, `art::art` vs `std::map` vs `std::unordered_map`)
```
delete:
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
      art_delete_sparse * | 1600000 |   711.271 |     444 |      - |  2249495.0
  red_black_delete_sparse | 1600000 |  1411.551 |     882 |  1.985 |  1133505.1
    hashmap_delete_sparse | 1600000 |   491.755 |     307 |  0.691 |  3253652.5
===============================================================================
insert:
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
      art_insert_sparse * | 1600000 |   609.976 |     381 |      - |  2623054.7
  red_black_insert_sparse | 1600000 |  1291.967 |     807 |  2.118 |  1238421.8
    hashmap_insert_sparse | 1600000 |   627.865 |     392 |  1.029 |  2548317.4
===============================================================================
mixed:
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
       art_mixed_sparse * | 1600000 |   662.734 |     414 |      - |  2414241.9
   red_black_mixed_sparse | 1600000 |  1209.072 |     755 |  1.824 |  1323328.8
     hashmap_mixed_sparse | 1600000 |   614.750 |     384 |  0.928 |  2602684.4
===============================================================================
query sparse uniform:
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
              art_q_s_u * | 1600000 |   710.420 |     444 |      - |  2252187.4
          red_black_q_s_u | 1600000 |  1077.434 |     673 |  1.517 |  1485009.8
            hashmap_q_s_u | 1600000 |   368.342 |     230 |  0.518 |  4343790.8
===============================================================================
query sparse uniform base 64 keys:
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
              art_q_s_u * | 1600000 |   713.069 |     445 |      - |  2243821.6
          red_black_q_s_u | 1600000 |  1704.574 |    1065 |  2.390 |   938650.9
            hashmap_q_s_u | 1600000 |   466.918 |     291 |  0.655 |  3426726.0
===============================================================================
query sparse zipf:
===============================================================================
   Name (baseline is *)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
              art_q_s_z * | 1600000 |   411.221 |     257 |      - |  3890849.2
          red_black_q_s_z | 1600000 |   747.299 |     467 |  1.817 |  2141044.3
            hashmap_q_s_z | 1600000 |   413.103 |     258 |  1.005 |  3873125.6
===============================================================================
```

You can replicate using `make release && make bench`

## Memory Benchmark

The `make bench-mem` command runs memory profiling using [Valgrind Massif](https://valgrind.org/docs/manual/ms-manual.html) to measure the memory footprint of the ART data structure. The benchmark tests two scenarios:

1. **Uniform distribution**: Keys are uniformly distributed
2. **Zipfian distribution**: Keys follow a Zipfian distribution (realistic workload with hot keys)

Both benchmarks insert 1,000,000 elements with `nullptr` values to measure only the data structure overhead.

Example usage:
```bash
# Build and run memory benchmarks
make release && make bench-mem
```

The command will output a summary of memory usage for both distributions and save detailed reports to:
- `build/massif.out.uniform` - Full report for uniform distribution
- `build/massif.out.zipf` - Full report for Zipfian distribution

To view the full reports:
```bash
ms_print build/massif.out.uniform
ms_print build/massif.out.zipf
```


## References

* [The Adaptive Radix Tree: ARTful Indexing for Main-Memory Databases](http://www-db.in.tum.de/~leis/papers/ART.pdf)
* [The Adaptive Radix Tree](http://rafaelkallis.com/static/media/the_adaptive_radix_tree_rafael_kallis.23779b20.pdf)
