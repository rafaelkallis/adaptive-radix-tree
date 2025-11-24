# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [6.0] - 2025-11-24 - "stable"

This is a major stability release with critical bug fixes and new memory profiling capabilities.

### Added
- **Memory Benchmarking** ([#31](https://github.com/rafaelkallis/adaptive-radix-tree/pull/31)) - Added `make bench-mem` command for memory profiling using Valgrind Massif
  - Tests uniform and Zipfian key distributions with 1M elements
  - Measures data structure overhead separate from value storage
  - Generates detailed memory usage reports
  - Thanks to [@hiqsociety](https://github.com/hiqsociety) for requesting this feature in [#16](https://github.com/rafaelkallis/adaptive-radix-tree/issues/16)
  - Implemented by [@Copilot](https://github.com/apps/copilot-swe-agent)

### Fixed
- **Buffer Overflow & Memory Leak** ([#30](https://github.com/rafaelkallis/adaptive-radix-tree/pull/30)) - Critical fixes in the `set` method
  - Fixed buffer overflow that wrote one byte beyond allocated memory
  - Fixed memory leak using wrong delete operator for arrays (`delete` → `delete[]`)
  - Added test documenting C-string semantics behavior
  - Thanks to [@Haibarapink](https://github.com/Haibarapink) for reporting in [#21](https://github.com/rafaelkallis/adaptive-radix-tree/issues/21)
  - Implemented by [@Copilot](https://github.com/apps/copilot-swe-agent)

- **Node Growth Bug** ([#29](https://github.com/rafaelkallis/adaptive-radix-tree/pull/29)) - Fixed `node_16::grow()` missing 128 offset
  - Children inserted before growth became unreachable after node_16→node_48 transition
  - Iterator navigation returned incorrect keys
  - Added comprehensive test for node growth scenarios
  - Thanks to [@bowling233](https://github.com/bowling233) for the fix (co-author)
  - Implemented by [@Copilot](https://github.com/apps/copilot-swe-agent)

- **Iterator Crash** ([#25](https://github.com/rafaelkallis/adaptive-radix-tree/pull/25)) - Fixed segmentation fault when iterating single-element trees
  - Fixed buffer underflow in `step::operator++()`
  - Fixed empty stack access in `seek_leaf()`
  - Added missing `#include <cstdint>` causing 138 compilation errors
  - Thanks to [@YelaShady](https://github.com/YelaShady) for reporting in [#19](https://github.com/rafaelkallis/adaptive-radix-tree/issues/19)
  - Implemented by [@Copilot](https://github.com/apps/copilot-swe-agent)

### Changed
- **Build System** ([#24](https://github.com/rafaelkallis/adaptive-radix-tree/pull/24)) - Increased CMake minimum version from 3.0 (2014) to 3.12 (2018)
  - Brings substantial improvements while maintaining C++11 compatibility
  - Implemented by [@Copilot](https://github.com/apps/copilot-swe-agent)

- **CI/CD Improvements** - Multiple enhancements by [@rafaelkallis](https://github.com/rafaelkallis)
  - Added GitHub Actions for automated builds and tests
  - Added devcontainer configuration for consistent development environment
  - Fixed debug build failures on aarch64 (forced signed char)
  - Resolved SIGSTKSZ compilation errors by disabling POSIX signals in doctest
  - Fixed uninitialized variable and dangling reference issues

### Contributors

A huge thank you to all contributors who made this release possible:

**Issue Reporters:**
- [@hiqsociety](https://github.com/hiqsociety) - Memory benchmarking feature request ([#16](https://github.com/rafaelkallis/adaptive-radix-tree/issues/16))
- [@Haibarapink](https://github.com/Haibarapink) - Buffer overflow bug report ([#21](https://github.com/rafaelkallis/adaptive-radix-tree/issues/21))
- [@YelaShady](https://github.com/YelaShady) - Iterator crash bug report ([#19](https://github.com/rafaelkallis/adaptive-radix-tree/issues/19))

**Code Contributors:**
- [@rafaelkallis](https://github.com/rafaelkallis) - Project maintainer, CI/CD improvements, build fixes
- [@bowling233](https://github.com/bowling233) - Co-author of node growth fix
- [@Copilot](https://github.com/apps/copilot-swe-agent) - Bug fixes and feature implementations
- [@copilot-swe-agent[bot]](https://github.com/apps/copilot-swe-agent) - Automated code contributions

**Release Manager:**
- [@copilot-swe-agent[bot]](https://github.com/apps/copilot-swe-agent) - Release preparation and changelog

### Statistics

- **30 commits** since version 5.0
- **5 pull requests** merged
- **3 critical bugs** fixed
- **1 new feature** added
- **26 tests** passing with 41,155,127 assertions

---

## [5.0] - Previous Release

See git history for changes in version 5.0 and earlier.
