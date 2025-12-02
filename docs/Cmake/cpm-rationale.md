# Design Rationale: Why CPM (C++ Package Manager)?

## Overview

The R-Type project uses **CPM (C++ Package Manager)** for dependency management instead of other package management solutions. This document explains the architectural and practical reasons behind this choice.

## Requirements Analysis

Before selecting CPM, we identified the following requirements for the R-Type project:

### Functional Requirements

- **Automatic dependency download** (no manual setup required)
- **Version pinning** (reproducible builds across machines)
- **Header-only and compiled library support** (flexibility)
- **Git repository integration** (pull latest or specific versions)
- **Offline build support** (cache dependencies locally)
- **Cross-platform compatibility** (Linux, Windows, macOS)
- **Minimal configuration** (simple CMake integration)

### Non-Functional Requirements

- **Zero external tools** (no additional package managers to install)
- **CMake-native solution** (leverage existing build system)
- **Fast dependency resolution** (minimal build startup time)
- **Low maintenance** (easy to update or remove dependencies)
- **No build system overhead** (lightweight integration)
- **Educational value** (understandable for team members)

## Comparison with Alternatives

### 1. **CPM** ✅ (Selected)

**Pros:**

- **Zero external dependencies**: No separate package manager to install (just CMake)
- **CMake-native**: Integrated directly into build pipeline
- **Simple API**: Single CMakeLists.txt call per dependency
- **Git support**: Direct integration with Git repositories (tags, branches, commits)
- **Automatic download**: Fetches dependencies on first build
- **Local caching**: Stores sources in `_deps` folder (offline builds)
- **Version control**: Pin exact versions for reproducibility
- **Header-only friendly**: No compilation overhead for header-only libs
- **FetchContent compatible**: Uses CMake's native FetchContent under the hood
- **Active maintenance**: Regular updates and bug fixes
- **MIT License**: Commercial-friendly license
- **Minimal learning curve**: Easy syntax for adding dependencies

**Cons:**

- Downloaded dependencies stored in build folder (not system-wide)
- Each project fetches dependencies independently (no global cache)
- Requires CMake 3.14+ (but standard nowadays)

**Why chosen for R-Type:**

- Game development workflow requires minimal setup friction
- No external tool dependencies align with educational environment
- Git-based dependencies allow precise version tracking
- Build isolation ensures consistency across team
- CMake integration avoids switching between build systems

---

### 2. **Conan** ❌ (Rejected)

**Pros:**

- Industry-standard C++ package manager
- Massive package ecosystem (thousands of pre-built packages)
- Binary caching (fast builds with precompiled libraries)
- Advanced version resolution
- Cross-compilation support

**Cons:**

- **Separate tool installation**: Requires installing Conan package manager
- **Steep learning curve**: Complex configuration files (conanfile.py, conan.lock)
- **Python dependency**: Conan requires Python (adds system requirement)
- **Overkill for small projects**: Designed for large enterprise setups
- **Setup friction**: Additional tool to install and configure
- **Version compatibility**: Conan versions can conflict with package versions
- **Educational overhead**: Too complex for learning-focused project

**Why rejected:**

- R-Type is an educational game project, not enterprise software
- Adding external tool dependencies conflicts with goal of "just clone and build"
- Complexity exceeds actual project needs
- Team members shouldn't need Conan knowledge to build R-Type
- Build isolation already achieved with CPM

---

### 3. **vcpkg** ❌ (Rejected)

**Pros:**

- Modern Microsoft-backed package manager
- Large package ecosystem
- Good cross-platform support
- Integration with Visual Studio

**Cons:**

- **Toolchain complexity**: Requires vcpkg installation and configuration
- **System-wide cache**: Dependencies stored in system folder (not project-isolated)
- **Integration friction**: Requires CMake toolchain file setup
- **Build variability**: System-wide cache can lead to inconsistent builds
- **Setup requirements**: Non-trivial configuration for Linux/macOS
- **Maintenance burden**: Toolchain updates can break builds

**Why rejected:**

- R-Type prioritizes project isolation (each build is independent)
- Additional toolchain complexity not justified for educational project
- Build reproducibility better served by local dependency storage

---

### 4. **Manual Git Submodules** ❌ (Rejected)

**Pros:**

- Native Git feature (no tools needed)
- Explicit version control (commits pinned)
- Direct repository access

**Cons:**

- **Manual management**: Requires `git submodule` commands
- **Clone complexity**: `git clone --recursive` needed for full setup
- **Merge conflicts**: Submodule pointers frequently conflict
- **Build integration**: No automatic CMake integration
- **Repository bloat**: Submodules add disk space and clone time
- **CI/CD friction**: Submodule setup errors in CI pipelines

**Why rejected:**

- Higher friction for new team members ("why is the folder empty?")
- Submodule conflicts during active development
- No CMake integration requires manual target setup
- Educational project shouldn't add Git complexity

---

### 5. **Hunter** ❌ (Rejected)

**Pros:**

- CMake-based package manager
- Good C++ library ecosystem
- Binary caching

**Cons:**

- **Maintenance decline**: Less active development
- **Complex configuration**: CMakeLists.txt setup is verbose
- **Slow builds**: Binary cache can be outdated
- **Community fragmentation**: Smaller community than Conan/vcpkg
- **Documentation gaps**: Fewer examples and guides

**Why rejected:**

- Project momentum declining compared to alternatives
- Simpler alternatives (CPM) more suitable for project scope

---

### 6. **Manual Dependency Management** ❌ (Rejected)

**Pros:**

- Complete control
- No external dependencies

**Cons:**

- **Maintenance nightmare**: Manual downloads and updates
- **Setup burden**: Each developer manually clones dependencies
- **Version inconsistency**: Easy to diverge between machines
- **Reproducibility issues**: Builds vary across team members
- **CI/CD complexity**: Complex scripting for dependency setup
- **Onboarding friction**: New members confused about setup

**Why rejected:**

- Violates DRY principle (Don't Repeat Yourself)
- No automation benefits
- Educational project should demonstrate best practices

---

## CPM: Detailed Advantages

### 1. **Zero External Dependencies**

Unlike Conan/vcpkg, CPM requires only **CMake** (already present):

```cmake
# That's it! No additional tools to install
include(cmake/CPM.cmake)

CPMAddPackage("gh:gabime/spdlog@1.14.1")
```

**Impact for R-Type:**

- `git clone && mkdir build && cd build && cmake .. && make`
- Single command workflow for new developers
- No Conan/vcpkg/Hunter installation required

### 2. **Git-Native Dependency Resolution**

CPM uses Git directly for version specification:

```cmake
# Pin exact version
CPMAddPackage("gh:gabime/spdlog@1.14.1")

# Or use branches
CPMAddPackage("gh:gabime/spdlog#v1.x")

# Or specific commits
CPMAddPackage("gh:gabime/spdlog#abc123def")
```

**Workflow benefits:**

- Developers understand Git references better than package manager versioning
- Version history visible in `cmake/CPM.cmake` (matches codebase history)
- Easy to test new library versions (just change the tag)

### 3. **Build Isolation**

Dependencies cached in `_deps` folder (build directory):

```
R-Type/
  build/
    _deps/
      spdlog/        # Downloaded here, not system-wide
      fmt/
      ...
```

**Advantages:**

- No system pollution
- Different projects can use different library versions
- Clean `rm -rf build` removes all dependencies
- Offline builds possible (cache preserved)

### 4. **CMake-Native Integration**

CPM extends CMake's `FetchContent` module:

```cmake
# Simple, familiar CMake syntax
include(cmake/CPM.cmake)

CPMAddPackage(
    NAME spdlog
    GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
    GIT_TAG "v1.14.1"
)

# Targets immediately available
target_link_libraries(R-Type PRIVATE spdlog::spdlog)
```

**No learning curve:**

- Team already knows CMake
- No new tools or languages to learn
- Single build system remains CMake

### 5. **Educational Value**

CPM demonstrates good practices:

```cmake
# Clear dependency declaration
# Version pinning visible
# Git-based, matches version control workflow
# No package manager complexity
```

**Perfect for educational context:**

- Students learn CMake build systems
- Dependency management concepts clear
- No abstraction layers obscuring the process

### 6. **CPM Bootstrap Pattern**

R-Type uses a two-tier approach:

```
cmake/CPM.cmake (Bootstrap layer)
  ├─ Downloads CPM itself from GitHub
  ├─ Caches it locally
  └─ Never needs manual editing
           ↓
CMakeLists.txt (Application layer)
  └─ Uses CPMAddPackage() to declare actual dependencies
```

**Key insight:**

- **`cmake/CPM.cmake`** = infrastructure (download CPM, done once)
- **`CMakeLists.txt`** = declarations (spdlog, fmt, other libs)
- **Workflow**: Add new dependency? Edit `CMakeLists.txt` only
- **Benefit**: `cmake/CPM.cmake` is a fire-and-forget bootstrap script

---

## Design Decisions Based on CPM

### 1. **Dependency Declaration Pattern**

R-Type uses a clean separation of concerns:

```cmake
# cmake/CPM.cmake - bootstraps CPM itself (never modified)
include(${CMAKE_BINARY_DIR}/cmake/CPM.cmake)

# CMakeLists.txt - declares actual dependencies
include(cmake/CPM.cmake)  # One-time setup

CPMAddPackage(
    NAME spdlog
    VERSION 1.12.0
    GITHUB_REPOSITORY gabime/spdlog
)
```

**Why this pattern:**

- `cmake/CPM.cmake` is a **bootstrap file** - download CPM once, never touch it again
- Actual dependencies declared in **CMakeLists.txt** (project root)
- New team members see all dependencies in the main CMakeLists.txt
- Updating library versions = single edit in project CMakeLists.txt
- Version history tracked in Git (CMakeLists.txt changes, not CPM.cmake)

### 2. **Reproducible Builds**

Exact version pinning ensures consistency:

```cmake
# Everyone gets spdlog 1.14.1
CPMAddPackage("gh:gabime/spdlog@1.14.1")

# Not "latest" or "compatible" - exact match
```

**Impact:**

- Developer machine builds = CI/CD builds
- No "works on my machine" problems
- Easier debugging (same library versions everywhere)

### 3. **Local Caching Strategy**

Dependencies stored in `build/_deps`:

```bash
# Developers can preserve cache between builds
rm -rf build/CMakeFiles build/CMakeCache.txt  # Keep _deps/
cmake .

# Or clean everything
rm -rf build
```

**Benefits:**

- Faster rebuilds (dependencies already downloaded)
- Offline work possible (after first build)
- Explicit cache management

---

## Conclusion

**CPM was selected because it uniquely satisfies R-Type's requirements:**

1. **Zero setup friction** (just CMake, nothing else to install)
2. **Git-native versioning** (aligns with developer workflow)
3. **Build isolation** (each project independent)
4. **CMake integration** (no new build systems to learn)
5. **Reproducible builds** (exact version pinning)
6. **Educational value** (demonstrates best practices clearly)
7. **Minimal maintenance** (simple CMakeLists.txt)
8. **Fast onboarding** (new developers get it immediately)

### Alternatives Rejected Because

- **Conan**: Requires external tool installation, steep learning curve, Python dependency, designed for enterprise scale
- **vcpkg**: System-wide cache breaks build isolation, complex toolchain setup, more friction than needed
- **Git Submodules**: Manual management, merge conflicts, CMake integration required, onboarding confusion
- **Hunter**: Declining maintenance, overly complex for project scope
- **Manual management**: No automation, version inconsistency, high maintenance burden
- **None (direct includes)**: No dependency tracking, no version management, chaos

CPM provides the optimal balance between **simplicity, automation, and control** for R-Type's game engine dependency management.

### Practical Example: Adding a New Dependency

With CPM, adding a new dependency is a **one-line edit**:

```cmake
# Before
CPMAddPackage("gh:gabime/spdlog@1.14.1")

# After adding fmt library
CPMAddPackage("gh:gabime/spdlog@1.14.1")
CPMAddPackage("gh:fmtlib/fmt@10.2.1")

# Then in CMakeLists.txt
target_link_libraries(R-Type PRIVATE spdlog::spdlog fmt::fmt)
```

**No other tools needed. No configuration files. No package manager queries. Just CMake.**

This simplicity is why CPM was chosen over more complex alternatives.
