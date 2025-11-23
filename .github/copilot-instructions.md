# Adaptive Radix Tree (ART) - AI Coding Instructions

## Project Overview
C++11 header-only library implementing the Adaptive Radix Tree data structure with vertical compression (prefix sharing) and horizontal compression (adaptive node sizing). Focus is on performance vs. `std::map` and `std::unordered_map`.

## Architecture & Node Hierarchy
- **Entry Point**: `include/art.hpp` includes all components. Use `art::art<T>` from `include/art/art.hpp`.
- **Node Polymorphism**: Abstract `node<T>` → `leaf_node<T>` (holds values) or `inner_node<T>` (base for internal nodes).
- **Adaptive Node Types** (horizontal compression):
  - `node_4`: 4 children (linear search in sorted `keys_[]`)
  - `node_16`: 16 children (linear search in sorted `keys_[]`)
  - `node_48`: 48 children (256-element `child_index_[]` maps partial keys to child positions)
  - `node_256`: 256 children (direct indexing `children_[partial_key]`)
- **Dynamic Resizing**: Nodes call `grow()`/`shrink()` to transition types (e.g., `node_4::grow()` → `node_16`). The old node is `delete`d, and the new node replaces it in-place via pointer-to-pointer (`**cur_inner`).

## Memory Management (Critical)
- **Manual Allocation**: All nodes use raw `new`/`delete` (no smart pointers).
- **Prefix Storage**: Each node has `char *prefix_` and `uint16_t prefix_len_` (vertical compression). Prefixes are allocated/deallocated manually.
- **Destructor Pattern**: `~art()` uses iterative traversal with `std::stack<node<T>*>` to avoid stack overflow on deep trees. Manually deletes `prefix_` and nodes.
- **Ownership**: The `art` class owns all nodes. User-provided values (`T`) are NOT owned—use pointers like `art<int*>` or `art<std::shared_ptr<T>>`.

## Build & Test Workflow
Uses CMake with a Makefile wrapper:

```bash
# Build debug (default)
make            # → build/test, build/bench, build/main

# Build release (optimizations)
make release    # → enables -O3 -DNDEBUG

# Run tests (doctest framework)
make test       # Runs build/test, must build first

# Run benchmarks (picobench framework)
make bench      # Runs build/bench, must build first

# Clean
make clean      # Removes build/
```

**Test Pattern**: `test/art.cpp` uses doctest `TEST_SUITE("art")` and `SUBCASE` for scenarios. Test against pointer types (`int*`).

**Benchmark Pattern**: `bench/*.cpp` files use `PICOBENCH()` macros. Compare `art::art` vs `std::map` vs `std::unordered_map`.

## Coding Conventions
- **C++11 only**: No newer features (constexpr, auto return types, etc.).
- **Namespace**: All code in `art::` namespace.
- **Naming**: `snake_case` for methods/variables, `_` suffix for members (`root_`, `n_children_`), uppercase `T` for templates.
- **Headers**: Template implementations are inline in `.hpp` files (header-only design).
- **Error Handling**: `assert()` for preconditions (e.g., `!is_full()` before `set_child()`). `nullptr` return for not-found. `std::runtime_error` for impossible states (e.g., `node_4::shrink()`).

## Key Implementation Patterns
- **Prefix Compression**: `node<T>::check_prefix(key, key_len)` uses `std::mismatch()` to find first divergence. Returns index where prefix and key differ.
- **Child Lookup**: `inner_node<T>::find_child(partial_key)` returns `node<T>**` (pointer-to-child-pointer) for in-place modification. Returns `nullptr` if not found.
- **Node Transitions**: When `is_full()`, caller invokes `*cur_inner = (**cur_inner).grow()` (replaces pointer). When `is_underfull()` after deletion, call `shrink()`.
- **Iterator**: `tree_it<T>` (in `include/art/tree_it.hpp`) provides lexicographical traversal via `art::begin()/end()`.

## Common Tasks
- **Adding Tests**: Append to `test/art.cpp` using `SUBCASE("description")` within `TEST_SUITE("art")`.
- **Adding Benchmarks**: Create `bench/<name>.cpp` with `PICOBENCH(function)`. Add to `CMakeLists.txt` under `bench` executable.
- **Modifying Node Logic**: Update all 4 node types (`node_4`, `node_16`, `node_48`, `node_256`) consistently. Check `grow()`/`shrink()` transitions.
- **Debugging Segfaults**: Check manual memory—ensure `prefix_` is deleted, nodes aren't double-freed, and `nullptr` checks are present after `find_child()`.
