# Release 6.0 - "stable"

**Release Date:** November 24, 2025

This is a major stability release with critical bug fixes and new memory profiling capabilities. We strongly recommend all users upgrade to this version.

## 🎯 Highlights

- **Critical Bug Fixes**: Fixed 3 severe bugs including buffer overflow, memory leak, and iterator crashes
- **Memory Profiling**: New `make bench-mem` command for detailed memory analysis
- **Improved Stability**: Comprehensive test coverage with 26 tests and 41M+ assertions
- **Modern Build System**: Updated CMake to version 3.12

## ✨ What's New

### Memory Benchmarking
Added comprehensive memory profiling using Valgrind Massif. Run `make bench-mem` to analyze the ART data structure's memory footprint with:
- Uniform key distribution tests
- Zipfian distribution tests (realistic workloads)
- 1 million element capacity testing
- Detailed memory usage reports

Example peak memory: ~78 MB (uniform), ~27 MB (Zipfian with 1M keys).

**Requested by:** @hiqsociety in [#16](https://github.com/rafaelkallis/adaptive-radix-tree/issues/16)  
**Implemented in:** [#31](https://github.com/rafaelkallis/adaptive-radix-tree/pull/31)

## 🐛 Critical Fixes

### Buffer Overflow & Memory Leak ([#30](https://github.com/rafaelkallis/adaptive-radix-tree/pull/30))
Fixed critical memory safety issues in the `set` method:
- **Buffer overflow**: Prevented writing beyond allocated memory bounds
- **Memory leak**: Fixed incorrect use of `delete` instead of `delete[]` for arrays
- Added comprehensive test coverage for C-string semantics

**Impact:** Could cause crashes, memory corruption, and resource leaks  
**Reported by:** @Haibarapink in [#21](https://github.com/rafaelkallis/adaptive-radix-tree/issues/21)

### Node Growth Bug ([#29](https://github.com/rafaelkallis/adaptive-radix-tree/pull/29))
Fixed `node_16::grow()` missing 128 offset in node_48 index population:
- Children became unreachable after node_16→node_48 transitions
- Iterator navigation returned incorrect keys
- Data effectively corrupted after specific growth patterns

**Impact:** Silent data corruption during tree growth  
**Co-authored by:** @bowling233

### Iterator Crash ([#25](https://github.com/rafaelkallis/adaptive-radix-tree/pull/25))
Fixed segmentation fault when iterating over single-element trees:
- Fixed buffer underflow in `step::operator++()`
- Fixed empty stack access in `seek_leaf()`
- Added missing `#include <cstdint>` (resolved 138 compilation errors)

**Impact:** Crashes when iterating trees with one element  
**Reported by:** @YelaShady in [#19](https://github.com/rafaelkallis/adaptive-radix-tree/issues/19)

## 🔧 Improvements

### CMake Version Bump ([#24](https://github.com/rafaelkallis/adaptive-radix-tree/pull/24))
Updated minimum CMake version from 3.0 (2014) to 3.12 (2018):
- Enables modern CMake features
- Widely available on current systems
- Maintains C++11 compatibility

### CI/CD Enhancements
Multiple improvements by @rafaelkallis:
- ✅ GitHub Actions for automated builds and tests
- 🐳 Devcontainer configuration for consistent development
- 🏗️ Fixed debug build failures on aarch64
- 🔧 Resolved SIGSTKSZ compilation errors
- 🐞 Fixed uninitialized variables and dangling references

## 📊 Statistics

- **30 commits** since version 5.0
- **5 pull requests** merged
- **3 critical bugs** fixed
- **1 major feature** added
- **26 test suites** passing
- **41,155,127 assertions** validated

## 🙏 Acknowledgments

### Issue Reporters
- @hiqsociety - Memory benchmarking feature request
- @Haibarapink - Buffer overflow discovery
- @YelaShady - Iterator crash report

### Code Contributors
- @rafaelkallis - Project maintainer, CI/CD, and build improvements
- @bowling233 - Co-author of node growth fix
- @Copilot - Bug fixes and feature implementations
- @copilot-swe-agent[bot] - Automated contributions and release preparation

## 🚀 Upgrade Recommendations

**High Priority:** Users experiencing:
- Crashes during iteration
- Memory leaks or corruption
- Incorrect data retrieval after insertions

**Medium Priority:** All other users should upgrade for:
- Improved stability
- Better memory profiling capabilities
- Modern build system support

## 📦 Installation

```bash
git clone --recurse-submodules https://github.com/rafaelkallis/adaptive-radix-tree
cd adaptive-radix-tree
make release
make test
```

## 📖 Documentation

See [CHANGELOG.md](CHANGELOG.md) for detailed changes.

---

**Full Changelog**: https://github.com/rafaelkallis/adaptive-radix-tree/compare/5.0...6.0
