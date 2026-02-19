# versions.md

## Capture Context
- Repository: `OpenSidescan`
- Branch: `master`
- Commit: `ad778d8`
- Capture timestamp: `2026-02-19 17:54:14 UTC`

## Versions Declared in Repository Files
| Item | Version / Value | Source |
|---|---|---|
| Minimum CMake (root) | `3.10` | `CMakeLists.txt:8` |
| Minimum CMake (Linux lock tests) | `2.8` | `test/linuxFileLockTest/CMakeLists.txt:1` |
| Root project version | empty (`set(PROJECT_VERSION)` with no value) | `CMakeLists.txt:11`, `CMakeLists.txt:12` |
| Root C++ standard | `C++17` | `CMakeLists.txt:15` |
| Test C++ standard | `C++17` | `test/CMakeLists.txt:9` |
| OpenCV requirement (Linux) | `4.5` | `CMakeLists.txt:125`, `test/CMakeLists.txt:44`, `test/linuxFileLockTest/CMakeLists.txt:7` |
| OpenCV requirement (Windows) | `4.5` | `CMakeLists.txt:159`, `test/win-fileLock-test/CMakeLists.txt:50` |
| Qt requirement (Windows) | `5.15` | `CMakeLists.txt:160` |
| Jenkins version scheme | `0.1.$BUILD_ID` | `Jenkinsfile:6`, `Jenkinsfile:8` |
| Packaged installer filename example | `Opensidescan-1.0.0-win64.exe` | `installer/Opensidescan-1.0.0-win64.exe` |
| CPack package version wiring | disabled/commented | `CMakeLists.txt:202` |
| Embedded test framework | Catch2 `2.11.0` | `test/catch.hpp:2`, `test/catch.hpp:16` |
| Model payload version | `crabtrapV1.onnx` | `CMakeLists.txt:23` |

## Versions Detected in the Environment
| Tool | Detected Version |
|---|---|
| `cmake` | `3.28.3` |
| `ctest` | `3.28.3` |
| `g++` | `13.3.0` |
| `make` | `4.3` |
| `doxygen` | `1.9.8` |
| `cppcheck` | `2.13.0` |
| `gcovr` | `8.6` |
| `python3` | `3.12.3` |
| `pkg-config` | `1.8.1` |

## Build/Test Validation
- Root configure: failed (`cmake -S . -B build-check-gha`)
  - Warning: project version is empty (`VERSION keyword ... expanded to nothing`).
  - Error: OpenCV package not found (`OpenCVConfig.cmake` / `opencv-config.cmake` missing).
- Linux unit-test script: failed (`bash Scripts/build_linux_unit_tests.sh`)
  - Root configure failed on missing OpenCV.
  - Test configure reported dataset download write issue for `/opt/XTF_Support_test_dataset.zip`.
  - Test configure failed on missing OpenCV, then `make` failed (`No rule to make target 'tests'`).
- Linux lock-test script: failed (`bash Scripts/build_lock_test.sh`)
  - Configure failed on missing OpenCV (`OpenCVConfig.cmake` / `opencv-config.cmake` missing).

## Versioning and CI Inconsistencies
1. No canonical project version source: root CMake version is empty.
2. Jenkins dynamic versioning (`0.1.$BUILD_ID`) conflicts with static installer filename convention (`1.0.0`).
3. CPack package version linkage is present but disabled.
4. Windows build and test rely on hardcoded local dependency paths (`C:/LIBS`, `C:/Qt`).
5. Linux unit-test CMake depends on internal/absolute dataset locations (`/opt`, internal URL references).

## Normalization Recommendations
1. Define canonical version in CMake (`project(OpenSidescan VERSION X.Y.Z)`) and wire it to CPack.
2. Reuse the canonical version in CI artifacts, tags, and installer naming.
3. Replace hardcoded Windows dependency paths with a reproducible toolchain strategy.
4. Move test dataset resolution to configurable, portable paths in CI.
