# SDFGen Build Guide (Windows & Linux)

**Complete guide for building SDFGen from source on Windows and Linux**

---

## Table of Contents

### Windows
1. [Understanding the Build Process](#understanding-the-build-process)
2. [Prerequisites](#prerequisites)
3. [Quick Start (5 Minutes)](#quick-start-5-minutes)
4. [The Three Build Scripts](#the-three-build-scripts)
5. [Complete First Build Walkthrough](#complete-first-build-walkthrough)
6. [Common Workflows](#common-workflows)
7. [Troubleshooting](#troubleshooting)
8. [Advanced Topics](#advanced-topics)

### Linux
9. [Linux Build Guide](#linux-build-guide)

---

## Understanding the Build Process

### What is CMake?

**CMake is a build system generator** - it doesn't compile your code directly. Instead, it:
1. Reads your project's `CMakeLists.txt` file
2. Generates build files for your compiler (Ninja or Visual Studio)
3. Those build files are then used to actually compile your code

Think of it like this:
```
Your Code + CMakeLists.txt  →  [CMake]  →  Build Files  →  [Compiler]  →  Executable
```

### The Two-Phase Build Process

**Every CMake build has TWO distinct phases:**

#### Phase 1: Configuration (Run Once)
**Purpose:** Generate build files

- **When:** First time, or after changing `CMakeLists.txt`
- **What it does:**
  - Detects your compiler (Visual Studio)
  - Finds dependencies (CUDA, libraries, etc.)
  - Reads `CMakeLists.txt` configuration
  - Generates build files in `build-Release/` or `build-Debug/`
- **Script:** `configure_cmake.bat`
- **Output:** Creates `build-Release/` directory with:
  - `CMakeCache.txt` - Configuration results
  - `build.ninja` or `.sln` files - Actual build instructions

#### Phase 2: Compilation (Run Many Times)
**Purpose:** Compile your code into executables

- **When:** Every time you change source code
- **What it does:**
  - Uses the build files from Phase 1
  - Compiles changed source files
  - Links object files into executable
  - Outputs `.exe` and `.lib` files
- **Script:** `build_with_vs.bat`
- **Output:** Your compiled programs in `build-Release/`

### Visual Build Flow

```
┌─────────────────────────────────────────────────────────┐
│  PHASE 1: CONFIGURATION (Once per project)             │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  1. setup_vs_env.bat                                   │
│     ↓                                                   │
│     Detects Visual Studio installation                 │
│     Sets up compiler environment                       │
│                                                         │
│  2. configure_cmake.bat                                │
│     ↓                                                   │
│     Reads: CMakeLists.txt                             │
│     Creates: build-Release/                            │
│     Generates: build.ninja (or .sln files)            │
│                                                         │
└─────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────┐
│  PHASE 2: COMPILATION (Every code change)              │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  3. build_with_vs.bat <target>                         │
│     ↓                                                   │
│     Reads: build.ninja                                 │
│     Compiles: *.cpp, *.cu → *.obj                     │
│     Links: *.obj → SDFGen.exe                       │
│                                                         │
└─────────────────────────────────────────────────────────┘
                          ↓
                    Run SDFGen.exe
```

### Key Concepts

| Term | Meaning | Example |
|------|---------|---------|
| **Source Files** | Your code (`.cpp`, `.h`, `.cu`) | `main.cpp`, `makelevelset3.cpp` |
| **Object Files** | Compiled but not linked (`.obj`) | `main.cpp.obj` |
| **Target** | An executable or library to build | `SDFGen`, `test_correctness`, `all` |
| **Build Directory** | Where compiled files go | `build-Release/` |
| **Generator** | Tool that compiles code | Ninja (fast) or Visual Studio |
| **Configuration** | Build mode | Release (optimized) or Debug (debuggable) |

### When to Reconfigure vs Rebuild

**Reconfigure (Phase 1) when:**
- ✅ First time building SDFGen
- ✅ You edit `CMakeLists.txt`
- ✅ You add/remove source files (sometimes)
- ✅ You change build configuration (Debug ↔ Release)
- ✅ You update dependencies (CUDA, libraries)

**Just Rebuild (Phase 2) when:**
- ✅ You edit `.cpp` or `.h` files
- ✅ Daily development work
- ✅ Fixing bugs
- ✅ Adding functions to existing files

---

## Prerequisites

### Required Software

#### 1. Visual Studio 2022

**What you need:**
- Any edition: Community (free), Professional, or Enterprise
- **Must install:** "Desktop development with C++" workload

**Verification:**
```bash
# Open VS Installer, check that this is installed:
☑ Desktop development with C++
```

**Download:** https://visualstudio.microsoft.com/downloads/

#### 2. CMake

**Usually included with Visual Studio**, but verify:

```bash
cmake --version
# Should output: cmake version 3.20.x or higher
```

If not installed: https://cmake.org/download/

#### 3. Ninja (Recommended)

**Included with Visual Studio**, usually at:
```
C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe
```

**To verify:**
```bash
ninja --version
# Should output: 1.10.x or higher
```

If missing: Download from https://github.com/ninja-build/ninja/releases

#### 4. Git Bash (Recommended for Windows)

**Why:** Makes running `.bat` scripts easier on Windows

**Download:** https://git-scm.com/downloads

### Project-Specific Prerequisites

**For GPU acceleration (optional):**
- NVIDIA CUDA Toolkit 12.x+
- NVIDIA GPU with compute capability 3.5+

**Verify CUDA:**
```bash
nvcc --version
# Should output: Cuda compilation tools, release 12.x
```

**Optional: VTK (for .vti output format):**
- VTK 9.0+ library
- Only needed if you want VTK ImageData output instead of binary .sdf

---

## Quick Start (5 Minutes)

**If you just want to build and don't care about details:**

### Step 0: Prerequisites Check
```bash
# Verify you have everything:
cmake --version    # Should show 3.20+
ninja --version    # Should show 1.10+
cl                 # Should show Microsoft C++ Compiler (if VS env is set)
```

### Step 1: Navigate to Tools
```bash
cd /c/path/to/SDFGenFast/tools
```

### Step 2: Configure (First Time Only)
```bash
./configure_cmake.bat Release
```
**Output:** Creates `build-Release/` directory

### Step 3: Build
```bash
./build_with_vs.bat all
```
**Output:** Creates `SDFGen.exe` in `build-Release/`

### Step 4: Run
```bash
cd ../build-Release
./SDFGen.exe mesh.stl 128
```

**That's it!** You're done. See below for details on what happened.

---

## The Three Build Scripts

### 1. setup_vs_env.bat

**Purpose:** Find and initialize Visual Studio compiler environment

**When to run:** Automatically called by other scripts (rarely manual)

**What it does:**
1. Searches for Visual Studio 2022 installation
2. Calls `vcvarsall.bat` to set up compiler paths
3. Sets environment variables: `VS_VERSION`, `VS_EDITION`, `VS_GENERATOR`
4. Makes `cl.exe` (compiler) and `ninja.exe` available

**Manual usage (if needed):**
```bash
./setup_vs_env.bat
# Now you have VS tools available in this shell
```

**Success looks like:**
```
[setup_vs_env] Found: Visual Studio 2022 Community
[setup_vs_env] Environment ready!
[setup_vs_env] Version:   Visual Studio 2022 Community
[setup_vs_env] Compiler:  x64
```

**Failure looks like:**
```
============================================
[setup_vs_env] ERROR: Visual Studio Not Found
============================================

Visual Studio 2022 is required to build SDFGen.

Installation Instructions:
  1. Download Visual Studio 2022 Community (FREE):
     https://visualstudio.microsoft.com/downloads/
...
```
→ Install Visual Studio 2022 with C++ workload

---

### 2. configure_cmake.bat

**Purpose:** Generate build files for your project (Phase 1)

**When to run:**
- First time building
- After editing `CMakeLists.txt`
- When switching Debug ↔ Release

**Syntax:**
```bash
configure_cmake.bat [build_type] [generator]
```

**Parameters:**
- `build_type`: `Release` (optimized) or `Debug` (debuggable) - **Default: Release**
- `generator`: `Ninja` (fast) or `"Visual Studio"` (IDE integration) - **Default: Ninja**

**Examples:**
```bash
# Standard release build (most common)
./configure_cmake.bat Release

# Debug build for debugging
./configure_cmake.bat Debug

# Release with VS solution files for IDE
./configure_cmake.bat Release "Visual Studio"

# Just "configure" uses default (Release + Ninja)
./configure_cmake.bat
```

**What it does (step by step):**
1. Calls `setup_vs_env.bat` to get compiler
2. Verifies `CMakeLists.txt` exists in parent directory
3. Creates `build-Release/` (or `build-Debug/`) directory
4. Runs: `cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..`
5. CMake:
   - Parses `CMakeLists.txt`
   - Finds compiler, CUDA, libraries
   - Generates `build.ninja` file
   - Creates `CMakeCache.txt` with configuration

**Success looks like:**
```
[configure_cmake] Running CMake...
-- The CXX compiler identification is MSVC 19.44.35207.0
-- The CUDA compiler identification is NVIDIA 12.4.131
-- Configuring done (2.1s)
-- Generating done (0.3s)
[configure_cmake] Configuration complete!
[configure_cmake] Build directory: C:\Apps\SDFGenFast\build-Release
```

**Output files in build-Release/:**
- `CMakeCache.txt` - Saved configuration
- `build.ninja` - Build instructions for Ninja
- `CMakeFiles/` - Internal CMake files

---

### 3. build_with_vs.bat

**Purpose:** Compile your code into executables (Phase 2)

**When to run:**
- After configuration
- Every time you change source code
- Multiple times per day during development

**Syntax:**
```bash
build_with_vs.bat <target> [build_type] [jobs]
```

**Parameters:**
- `target`: **REQUIRED** - What to build
  - `all` - Build everything
  - `SDFGen_name` - Build specific target
  - `clean` - Delete compiled files
- `build_type`: `Release` or `Debug` - **Default: Release**
- `jobs`: Number of parallel jobs - **Default: CPU core count**

**Examples:**
```bash
# Build everything (most common)
./build_with_vs.bat all

# Build specific target
./build_with_vs.bat SDFGen

# Debug build
./build_with_vs.bat all Debug

# Limit to 4 parallel jobs
./build_with_vs.bat all Release 4

# Clean build artifacts
./build_with_vs.bat clean
```

**What it does (step by step):**
1. Calls `setup_vs_env.bat` to get compiler
2. Verifies `build-Release/CMakeCache.txt` exists
3. Runs: `cmake --build . --target all -j 24`
4. Ninja (or MSBuild):
   - Reads `build.ninja`
   - Compiles changed `.cpp` files → `.obj` files
   - Links `.obj` files → `.exe` or `.lib`
   - Outputs to `build-Release/`

**Success looks like:**
```
[build_with_vs] Building...
[1/15] Building CXX object main.cpp.obj
[2/15] Building CXX object makelevelset3.cpp.obj
...
[15/15] Linking CXX executable SDFGen.exe
[build_with_vs] SUCCESS: Build completed for 'all'
```

**Output files in build-Release/:**
- `SDFGen.exe` - Your executable
- `*.obj` - Object files (intermediate)
- `*.lib` - Static libraries (if any)

---

## Complete First Build Walkthrough

**Scenario:** You've just downloaded a CMake project and want to build it.

### Step-by-Step with Explanations

#### Step 1: Verify Prerequisites

```bash
# Check Visual Studio
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
# Should initialize without errors

# Check CMake
cmake --version
# Output: cmake version 3.24.2 (or higher)

# Check Ninja
ninja --version
# Output: 1.11.1 (or higher)
```

**If any fail:** Install missing software from [Prerequisites](#prerequisites)

---

#### Step 2: Examine Project Structure

```bash
cd /c/path/to/SDFGenFast

# Verify structure
ls
```

**You should see:**
```
CMakeLists.txt    ← REQUIRED - This is the build configuration
src/              ← Your source code
include/          ← Header files
tools/            ← Build scripts (this guide!)
  ├── setup_vs_env.bat
  ├── configure_cmake.bat
  ├── build_with_vs.bat
  └── BUILD_GUIDE.md
```

**If no tools/ directory:** Copy scripts from another SDFGen installation or this repository

---

#### Step 3: Navigate to Tools Directory

```bash
cd tools
pwd
# Output: /c/path/to/SDFGenFast/tools
```

**Why tools/ directory?**
- Scripts use relative paths: `..` = project root
- Keeps build scripts organized
- Matches convention of this guide

---

#### Step 4: Configure the Build (Phase 1)

```bash
./configure_cmake.bat Release
```

**What's happening (watch the output):**

```
[setup_vs_env] Detecting Visual Studio installation...
```
→ Finding your compiler

```
[setup_vs_env] Found: Visual Studio 2022 Community
[setup_vs_env] Initializing x64 environment...
```
→ Setting up compiler paths

```
[configure_cmake] Verifying CMakeLists.txt exists...
```
→ Checking project structure

```
[configure_cmake] Running CMake...
-- The CXX compiler identification is MSVC 19.44
```
→ CMake detecting compiler

```
-- Configuring done (2.5s)
-- Generating done (0.4s)
```
→ CMake generating build files

```
[configure_cmake] Configuration complete!
[configure_cmake] Build directory: C:\Apps\SDFGenFast\build-Release
```
→ Success! Build files created

**Check the output:**
```bash
cd ../build-Release
ls
```

**You should see:**
```
CMakeCache.txt           ← Configuration saved here
build.ninja              ← Ninja build instructions
CMakeFiles/              ← CMake internal files
```

---

#### Step 5: Build the Code (Phase 2)

```bash
cd ../tools  # Back to tools directory
./build_with_vs.bat all
```

**What's happening (watch the output):**

```
[build_with_vs] Build Configuration
[build_with_vs] Target:     all
[build_with_vs] Build Type: Release
[build_with_vs] Jobs:       24
```
→ Build parameters confirmed

```
[1/15] Building CXX object CMakeFiles\SDFGen.dir\main.cpp.obj
```
→ Compiling main.cpp to object file

```
[2/15] Building CXX object CMakeFiles\SDFGen.dir\makelevelset3.cpp.obj
[3/15] Building CXX object CMakeFiles\SDFGen.dir\mesh_io.cpp.obj
```
→ Compiling other source files (in parallel!)

```
[14/15] Linking CXX executable cmake_device_link.obj
```
→ Linking CUDA device code (if CUDA project)

```
[15/15] Linking CXX executable SDFGen.exe
```
→ Final linking to create executable

```
[build_with_vs] SUCCESS: Build completed for 'all'
```
→ Done!

**Verify the output:**
```bash
cd ../build-Release
ls *.exe
```

**You should see:**
```
SDFGen.exe    ← Your compiled program!
```

---

#### Step 6: Run Your Program

```bash
./SDFGen.exe mesh.stl 128
```

**Output:**
```
SDFGen - SDF Generation Tool
========================================

Usage: SDFGen <file> <parameters>

Mode 1: SDFGen <file.obj> <dx> <padding> [threads]
Mode 2a: SDFGen <file.stl> <Nx> [padding] [threads]
Mode 2b: SDFGen <file.stl> <Nx> <Ny> <Nz> [padding] [threads]
```

**Success!** Your program is built and running.

---

### What Just Happened? (Summary)

1. **Configuration Phase:**
   - CMake read `CMakeLists.txt`
   - Found Visual Studio compiler
   - Generated `build.ninja` with build instructions
   - Created `build-Release/` directory

2. **Compilation Phase:**
   - Ninja read `build.ninja`
   - Compiled each `.cpp` file to `.obj`
   - Linked all `.obj` files into `SDFGen.exe`

3. **Result:**
   - Executable ready to run: `build-Release/SDFGen.exe`

---

## Common Workflows

### Daily Development Workflow

**Scenario:** You're working on the code daily, fixing bugs, adding features.

```bash
# 1. Edit source code in your editor
# (e.g., modify src/main.cpp)

# 2. Rebuild (no reconfigure needed!)
cd tools
./build_with_vs.bat all

# 3. Test
cd ../build-Release
./SDFGen.exe
```

**Key point:** You only need `build_with_vs.bat`, not `configure_cmake.bat`!

---

### After Editing CMakeLists.txt

**Scenario:** You added a new source file or changed build settings.

```bash
# 1. Edit CMakeLists.txt
# (e.g., add new .cpp file to add_executable)

# 2. Reconfigure
cd tools
./configure_cmake.bat Release

# 3. Rebuild
./build_with_vs.bat all

# 4. Test
cd ../build-Release
./SDFGen.exe
```

**Why reconfigure?** CMake needs to regenerate build files with new source list.

---

### Building Specific Targets

**Scenario:** Want to build specific test instead of all targets, you only want one.

```bash
# Find available targets
cd build-Release
cmake --build . --target help

# Build specific target
cd ../tools
./build_with_vs.bat my_specific_app

# Result: only my_specific_app.exe is built
```

**Saves time:** Doesn't rebuild unrelated code.

---

### Debug vs Release Builds

**Scenario:** Need debug build for debugging, release for performance testing.

```bash
# Configure both
cd tools
./configure_cmake.bat Debug      # Creates build-Debug/
./configure_cmake.bat Release    # Creates build-Release/

# Build debug
./build_with_vs.bat all Debug    # Uses build-Debug/
cd ../build-Debug
./SDFGen.exe                   # Debug version (larger, slower, debuggable)

# Build release
cd ../tools
./build_with_vs.bat all Release  # Uses build-Release/
cd ../build-Release
./SDFGen.exe                   # Release version (smaller, faster, optimized)
```

**Keep both:** Directories are independent, can coexist.

---

### Clean Rebuild

**Scenario:** Build is acting weird, want to start fresh.

**Option 1: Clean target**
```bash
cd tools
./build_with_vs.bat clean        # Deletes .obj, .exe
./build_with_vs.bat all          # Fresh compile
```

**Option 2: Delete build directory**
```bash
rm -rf ../build-Release
./configure_cmake.bat Release    # Recreate from scratch
./build_with_vs.bat all
```

**Option 2 is more thorough** - use if Option 1 doesn't fix issues.

---

### Incremental vs Full Rebuild

**Incremental (default):**
```bash
./build_with_vs.bat all
# Only recompiles changed files (fast!)
```

**Full rebuild:**
```bash
./build_with_vs.bat clean
./build_with_vs.bat all
# Recompiles everything (slow but thorough)
```

**When to full rebuild:**
- After updating dependencies
- When getting weird linker errors
- After pulling major changes from git

---

## Troubleshooting

### Troubleshooting by Phase

#### Configuration Phase Errors

**Error:** `Visual Studio Not Found`

**Cause:** `setup_vs_env.bat` can't find Visual Studio 2022

**Fix:**
1. Verify installation:
   ```bash
   ls "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
   ```
2. If missing: Install Visual Studio 2022 with "Desktop development with C++"
3. If custom location: Edit `setup_vs_env.bat` line 22-29 with your path

---

**Error:** `CMakeLists.txt not found in ...`

**Cause:** Scripts expect to be in `ProjectRoot/tools/`

**Fix:** Check directory structure:
```
✓ Correct:
  ProjectRoot/
  ├── CMakeLists.txt
  └── tools/
      └── configure_cmake.bat

✗ Wrong:
  ProjectRoot/
  └── tools/
      ├── CMakeLists.txt     ← Should be one level up!
      └── configure_cmake.bat
```

Move `CMakeLists.txt` to project root, or move scripts to same directory.

---

**Error:** `CUDA toolset not found`

**Cause:** CMake can't find CUDA Toolkit

**Fix:**
1. Verify CUDA installed:
   ```bash
   nvcc --version
   ```
2. If missing: Install from https://developer.nvidia.com/cuda-downloads
3. Restart terminal after installation
4. Use Ninja generator (better CUDA support):
   ```bash
   ./configure_cmake.bat Release Ninja
   ```

---

#### Compilation Phase Errors

**Error:** `Build directory not configured`

**Cause:** Tried to build before configuring

**Fix:**
```bash
cd tools
./configure_cmake.bat Release   # Configure first!
./build_with_vs.bat all          # Then build
```

---

**Error:** `fatal error C1083: Cannot open include file: 'xxx.h'`

**Cause:** Missing dependency or wrong include paths

**Fix:**
1. Install missing library
2. Update `CMakeLists.txt` with correct paths:
   ```cmake
   include_directories(/path/to/includes)
   ```
3. Reconfigure:
   ```bash
   ./configure_cmake.bat Release
   ./build_with_vs.bat all
   ```

---

**Error:** `LNK1104: cannot open file 'xxx.lib'`

**Cause:** Missing library file or wrong library paths

**Fix:**
1. Install missing library
2. Update `CMakeLists.txt`:
   ```cmake
   link_directories(/path/to/libs)
   ```
3. Reconfigure and rebuild

---

**Error:** `ninja: build stopped: subcommand failed`

**Cause:** Compilation error in your code

**Fix:**
1. Scroll up in output to find actual error (usually above this message)
2. Fix the code error
3. Rebuild:
   ```bash
   ./build_with_vs.bat all
   ```

---

### Performance Issues

**Build is very slow**

**Causes & Fixes:**

1. **Not using Ninja:**
   ```bash
   ./configure_cmake.bat Release Ninja  # Ninja is faster than MSBuild
   ```

2. **Not using parallel builds:**
   ```bash
   ./build_with_vs.bat all Release 16   # Use more cores
   ```

3. **Antivirus scanning build directory:**
   - Add `build-Release/` to antivirus exclusions
   - Huge speedup on Windows!

4. **Building Debug instead of Release:**
   ```bash
   ./build_with_vs.bat all Release  # Release is faster to compile
   ```

---

## Advanced Topics

### Understanding CMake Generators

**What is a generator?**
CMake doesn't compile code - it generates build files for a "generator" that does.

**Two options:**

| Generator | Build Files | Best For | Speed |
|-----------|-------------|----------|-------|
| **Ninja** | `build.ninja` | Command-line builds, CI/CD | ⚡ Fast |
| **Visual Studio** | `.sln`, `.vcxproj` | Visual Studio IDE integration | 🐢 Slower |

**Use Ninja unless:**
- You want to open the project in Visual Studio IDE
- You need `.sln` files for some tool

**Switching generators:**
```bash
# Delete old build directory
rm -rf ../build-Release

# Reconfigure with different generator
./configure_cmake.bat Release "Visual Studio"

# Build
./build_with_vs.bat all
```

---

### Custom CMake Options

**Scenario:** Your project has CMake options like `-DENABLE_TESTS=ON`

**Method 1: Edit configure_cmake.bat**

Open `tools/configure_cmake.bat`, find the cmake command (around line 65):

```batch
REM Add your options here:
cmake -G Ninja ^
  -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
  -DENABLE_TESTS=ON ^
  -DUSE_CUDA=ON ^
  "%PROJECT_ROOT%"
```

**Method 2: Set environment variable**

```bash
export CMAKE_OPTIONS="-DENABLE_TESTS=ON -DUSE_CUDA=ON"
./configure_cmake.bat Release
```

(Requires modifying script to read this variable)

**Method 3: Run cmake directly**

```bash
cd ../build-Release
cmake -DENABLE_TESTS=ON -DUSE_CUDA=ON ..
cd ../tools
./build_with_vs.bat all
```

---

### Finding Available Targets

**List all targets:**
```bash
cd build-Release
cmake --build . --target help
```

**Output:**
```
The following are some of the valid targets for this Makefile:
... all (the default if no target is provided)
... clean
... SDFGen
... sdfgen_common
```

**Or:** Look in `CMakeLists.txt` for `add_executable()` and `add_library()`:
```cmake
add_executable(SDFGen main.cpp)         # Target: SDFGen
add_library(sdfgen_common STATIC sdfgen_common.cpp)      # Target: sdfgen_common
```

---

### CI/CD Integration

**GitHub Actions example:**

```yaml
name: Build

on: [push, pull_request]

jobs:
  build-windows:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v3

      - name: Configure
        run: tools\configure_cmake.bat Release
        shell: cmd

      - name: Build
        run: tools\build_with_vs.bat all
        shell: cmd

      - name: Test
        run: build-Release\SDFGen.exe --test
        shell: cmd
```

---

### Multiple Build Configurations

**Maintain Debug and Release side-by-side:**

```bash
cd tools

# Configure both
./configure_cmake.bat Debug      # → build-Debug/
./configure_cmake.bat Release    # → build-Release/

# Build both
./build_with_vs.bat all Debug    # Uses build-Debug/
./build_with_vs.bat all Release  # Uses build-Release/

# Each has its own executables
ls ../build-Debug/*.exe
ls ../build-Release/*.exe
```

**Benefit:** Switch between debug and release without reconfiguring

---

## Quick Reference

### Command Cheat Sheet

```bash
# First time build (3 commands)
cd /c/path/to/Project/tools
./configure_cmake.bat Release
./build_with_vs.bat all

# Daily rebuild (1 command)
./build_with_vs.bat all

# After changing CMakeLists.txt (2 commands)
./configure_cmake.bat Release
./build_with_vs.bat all

# Clean rebuild
./build_with_vs.bat clean
./build_with_vs.bat all

# Debug build
./configure_cmake.bat Debug
./build_with_vs.bat all Debug

# Specific target
./build_with_vs.bat my_target
```

---

### File Locations Reference

```
SDFGenFast/
├── CMakeLists.txt              ← Build configuration
├── src/                        ← Your source code
│   ├── main.cpp
│   └── makelevelset3.cpp
├── include/                    ← Header files
│   └── FileIO.h
├── tools/                      ← Build scripts
│   ├── setup_vs_env.bat
│   ├── configure_cmake.bat
│   ├── build_with_vs.bat
│   └── BUILD_GUIDE.md
├── build-Release/              ← Generated by configure
│   ├── CMakeCache.txt          ← Configuration saved
│   ├── build.ninja             ← Build instructions
│   ├── *.obj                   ← Object files
│   └── SDFGen.exe            ← Final executable
└── build-Debug/                ← Debug build (if configured)
    └── SDFGen.exe            ← Debug executable
```

---

## Summary

### Build Process Recap

1. **Once:** Configure with `configure_cmake.bat`
   - Generates build files
   - Run once per project, or when CMakeLists.txt changes

2. **Many times:** Build with `build_with_vs.bat`
   - Compiles source code
   - Run every time you change code

3. **Run:** Execute `build-Release/SDFGen.exe`

### The Three Scripts

1. `setup_vs_env.bat` - Find Visual Studio (automatic)
2. `configure_cmake.bat` - Generate build files (once)
3. `build_with_vs.bat` - Compile code (many times)

### Key Takeaways

- ✅ **Two phases:** Configure once, build many
- ✅ **Build directory:** Output goes to `build-Release/`
- ✅ **Reconfigure when:** Changing `CMakeLists.txt`
- ✅ **Just rebuild when:** Changing source code
- ✅ **Ninja is faster:** Use it unless you need IDE integration
- ✅ **Clean rebuild fixes:** Many weird errors

---

## Getting Help

**If something doesn't work:**

1. **Read the error message** - It usually tells you what's wrong
2. **Check this guide's Troubleshooting section**
3. **Verify prerequisites** - Visual Studio, CMake, Ninja installed?
4. **Try clean rebuild** - Delete `build-Release/`, reconfigure
5. **Check CMakeLists.txt** - Is it correct for SDFGen?

**Still stuck?**
- SDFGen-specific issues: Check README.md or GitHub issues
- CMake questions: https://cmake.org/documentation/
- Visual Studio: https://docs.microsoft.com/cpp/

---

**Last Updated:** 2025-10-30
**Version:** 2.1 - Added Linux support with auto-detection
**Tested:**
- Windows: Visual Studio 2022, CMake 3.24+, Ninja 1.11+
- Linux: Ubuntu 22.04 LTS, GCC 11+, CMake 3.20+

---

# Linux Build Guide

## Quick Start (Linux)

**If you just want to build on Linux:**

```bash
# 1. Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake ninja-build

# 2. Configure (auto-detects CUDA)
cd tools
chmod +x configure.sh build.sh
./configure.sh Release

# 3. Build
./build.sh

# 4. Run
./build-Release/bin/SDFGen mesh.stl 128
```

## Linux Prerequisites

### Essential Tools

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install \
    build-essential \
    cmake \
    git

# Fedora/RHEL
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake git

# Arch Linux
sudo pacman -S base-devel cmake git
```

### Optional: Ninja (Recommended)

```bash
# Ubuntu/Debian
sudo apt-get install ninja-build

# Fedora
sudo dnf install ninja-build

# Arch
sudo pacman -S ninja
```

### Optional: CUDA Toolkit (for GPU acceleration)

**Ubuntu/Debian:**
```bash
# Check if already installed
nvcc --version

# If not installed:
# 1. Download from https://developer.nvidia.com/cuda-downloads
# 2. Follow installer instructions
# 3. Add to PATH:
echo 'export PATH=/usr/local/cuda/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
```

**Post-installation verification:**
```bash
nvcc --version
# Should output: Cuda compilation tools, release 12.x
```

### Optional: VTK (for .vti output)

```bash
# Ubuntu/Debian
sudo apt-get install libvtk9-dev

# Check if installed
pkg-config --modversion vtk
```

## The Two Linux Build Scripts

### 1. configure.sh - Configuration Script

**Purpose:** Auto-detect CUDA and generate build files

**Syntax:**
```bash
./configure.sh [Release|Debug]
```

**What it does:**
1. Detects CMake version (requires 3.20+)
2. Chooses build system (Ninja if available, otherwise Make)
3. **Automatically detects CUDA Toolkit** (no manual flags!)
4. Detects VTK if installed
5. Generates build files in `build-Release/` or `build-Debug/`

**Examples:**
```bash
# Release build (default, optimized)
./configure.sh Release

# Debug build (with debug symbols)
./configure.sh Debug

# Just "configure" uses Release
./configure.sh
```

**Success output:**
```
========================================
SDFGen - Configuration Script
========================================

System Information:
  OS: Linux
  Architecture: x86_64
  Build Type: Release

CMake: 3.22.1
Build System: Ninja (fast parallel builds)

CUDA Detection:
  ✓ CUDA Toolkit found: 12.2
  ✓ nvcc: /usr/local/cuda/bin/nvcc

Build Configuration: CPU + GPU (CUDA acceleration enabled)
  GPU support will be compiled and used automatically at runtime

Optional Dependencies:
  ✓ VTK found (enables .vti output format)

========================================
Creating Build Directory
========================================

Created: /home/user/SDFGenFast/build-Release

========================================
Running CMake Configuration
========================================

-- The CXX compiler identification is GNU 11.4.0
-- The CUDA compiler identification is NVIDIA 12.2.140
...
-- Configuring done (2.3s)
-- Generating done (0.5s)

========================================
Configuration Complete
========================================

Build directory: /home/user/SDFGenFast/build-Release
Build type: Release
Generator: Ninja

Next steps:
  1. Build:  cd tools && ./build.sh
  2. Test:   cd build-Release && ctest --output-on-failure
  3. Run:    ./build-Release/bin/SDFGen <args>
```

---

### 2. build.sh - Compilation Script

**Purpose:** Compile code into executables

**Syntax:**
```bash
./build.sh [target] [Release|Debug]
```

**Parameters:**
- `target`: What to build (default: `all`)
  - `all` - Build everything
  - `SDFGen` - Build main application only
  - `test_cli_backend` - Build specific test
- `build_type`: `Release` or `Debug` (default: `Release`)

**Examples:**
```bash
# Build everything
./build.sh

# Build main application only
./build.sh SDFGen

# Build specific test
./build.sh test_cli_backend

# Build in Debug mode
./build.sh all Debug
```

**What it does:**
1. Verifies configuration exists
2. Detects CPU count for parallel compilation
3. Runs: `cmake --build . --target <target> -j <cpus>`
4. Outputs binaries to `build-Release/bin/`

**Success output:**
```
============================================
Build Configuration
============================================
Target:     all
Build Type: Release
Build Dir:  /home/user/SDFGenFast/build-Release
Generator:  Ninja
Jobs:       16
============================================

Building...

[1/45] Building CXX object common/CMakeFiles/sdfgen_common.dir/sdfgen_unified.cpp.o
[2/45] Building CXX object cpu_lib/CMakeFiles/sdfgen_cpu.dir/makelevelset3.cpp.o
[3/45] Building CUDA object gpu_lib/CMakeFiles/sdfgen_gpu.dir/makelevelset3_gpu.cu.o
...
[44/45] Linking CXX executable bin/SDFGen
[45/45] Linking CXX executable bin/test_cli_backend

============================================
SUCCESS: Build completed for 'all'
============================================

Executable: /home/user/SDFGenFast/build-Release/bin/SDFGen

Usage:
  /home/user/SDFGenFast/build-Release/bin/SDFGen <mesh.stl> <Nx> [padding]
  /home/user/SDFGenFast/build-Release/bin/SDFGen <mesh.obj> <dx> <padding>
```

## Complete Linux Build Walkthrough

### Step 1: Install Prerequisites

```bash
# Update package list
sudo apt-get update

# Essential tools
sudo apt-get install build-essential cmake git

# Recommended: Ninja for faster builds
sudo apt-get install ninja-build

# Optional: CUDA for GPU acceleration
# Download from https://developer.nvidia.com/cuda-downloads
# Follow installer, then:
echo 'export PATH=/usr/local/cuda/bin:$PATH' >> ~/.bashrc
source ~/.bashrc

# Optional: VTK for .vti output
sudo apt-get install libvtk9-dev
```

**Verify installation:**
```bash
g++ --version      # Should show GCC 9.0+
cmake --version    # Should show 3.20+
ninja --version    # Should show 1.10+ (if installed)
nvcc --version     # Should show CUDA 11.0+ (if installed)
```

### Step 2: Clone/Download Project

```bash
cd ~/
git clone https://github.com/your/project.git
cd project
```

**Verify structure:**
```bash
ls
# Should see:
# CMakeLists.txt
# tools/
# app/
# common/
# cpu_lib/
# gpu_lib/
```

### Step 3: Make Scripts Executable

```bash
cd tools
chmod +x configure.sh build.sh
```

**Why?** Git doesn't always preserve executable permissions on Windows→Linux

### Step 4: Configure

```bash
./configure.sh Release
```

**Watch the output:**
- ✓ CUDA detected → GPU support enabled
- ✗ CUDA not found → CPU-only build (still works!)
- ✓ VTK found → .vti output available
- ✗ VTK not found → Only binary .sdf output

### Step 5: Build

```bash
./build.sh
```

**This will:**
- Compile all C++ and CUDA source files
- Link libraries
- Create executables in `build-Release/bin/`

**Build time:**
- CPU-only: ~30 seconds
- With CUDA: ~60 seconds (first time)
- Incremental rebuilds: ~5 seconds

### Step 6: Verify

```bash
cd ../build-Release/bin
ls -lh
# Should see:
# SDFGen              (main executable)
# test_cli_backend    (test executables)
# ...
```

### Step 7: Run

```bash
./SDFGen mesh.stl 128
```

**Output:**
```
SDFGen - A utility for converting closed oriented triangle meshes into grid-based signed distance fields.

=== Mode 1: Legacy OBJ with dx spacing ===
Usage: SDFGen <file.obj> <dx> <padding>
...

=== Hardware Acceleration ===
GPU acceleration (CUDA) is used automatically if available.
The program will detect and report which backend is being used.
No special flags needed - it just works!
```

**Test with sample mesh:**
```bash
./SDFGen ../../tests/resources/test_x3y4z5_bin.stl 128
```

**Output should show:**
```
Hardware: GPU acceleration available
Implementation: GPU (CUDA)
```

or

```
Hardware: No CUDA GPU detected
Implementation: CPU (multi-threaded)
```

## Linux Common Workflows

### Daily Development

```bash
# 1. Edit source code
vim ../app/main.cpp

# 2. Rebuild (no reconfigure needed!)
cd tools
./build.sh

# 3. Test
./build-Release/bin/SDFGen test.stl 64
```

### After Editing CMakeLists.txt

```bash
# 1. Edit CMakeLists.txt
vim ../CMakeLists.txt

# 2. Reconfigure
cd tools
./configure.sh Release

# 3. Rebuild
./build.sh
```

### Debug Build

```bash
# Configure debug build
cd tools
./configure.sh Debug

# Build
./build.sh all Debug

# Debug with GDB
cd ../build-Debug/bin
gdb ./SDFGen
(gdb) run test.stl 64
```

### Clean Rebuild

```bash
# Method 1: Clean target
cd tools
./build.sh clean
./build.sh

# Method 2: Delete build directory (more thorough)
rm -rf ../build-Release
./configure.sh Release
./build.sh
```

### Running Tests

```bash
# Run all tests
cd build-Release
ctest --output-on-failure

# Run specific test
cd ../tests
../build-Release/bin/test_cli_backend

# Or from tools directory
../build-Release/bin/test_correctness
```

## Linux Troubleshooting

### Error: "cmake: command not found"

**Fix:**
```bash
sudo apt-get install cmake

# Verify
cmake --version
```

### Error: "g++: command not found"

**Fix:**
```bash
sudo apt-get install build-essential

# Verify
g++ --version
```

### Error: "Permission denied: ./configure.sh"

**Fix:**
```bash
chmod +x configure.sh build.sh
./configure.sh Release
```

### Error: "nvcc: command not found" (but CUDA is installed)

**Fix:**
```bash
# Add CUDA to PATH
echo 'export PATH=/usr/local/cuda/bin:$PATH' >> ~/.bashrc
echo 'export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc

# Verify
nvcc --version
```

### Error: "undefined reference to cudaGetDeviceCount"

**Cause:** CUDA libraries not linked properly

**Fix:**
```bash
# Reconfigure to detect CUDA
cd tools
rm -rf ../build-Release
./configure.sh Release
./build.sh
```

### Build is slow

**Fix 1: Install Ninja**
```bash
sudo apt-get install ninja-build
rm -rf ../build-Release
./configure.sh Release  # Will auto-detect Ninja
./build.sh
```

**Fix 2: Use more CPU cores**
```bash
# Edit build.sh or run manually:
cmake --build . -j $(nproc)
```

### GPU not detected at runtime

**Check:**
```bash
# Verify NVIDIA driver
nvidia-smi

# Verify CUDA runtime
ldd ./build-Release/bin/SDFGen | grep cuda

# Should show: libcudart.so => /usr/local/cuda/lib64/libcudart.so
```

**Fix:**
```bash
# Install NVIDIA driver
ubuntu-drivers devices
sudo ubuntu-drivers autoinstall
sudo reboot

# Verify
nvidia-smi
```

## Linux vs Windows Differences

| Feature | Linux | Windows |
|---------|-------|---------|
| **Scripts** | `.sh` (bash) | `.bat` (cmd) |
| **Build System** | Ninja/Make | Ninja/MSBuild |
| **Compiler** | GCC/Clang | MSVC |
| **CUDA Detection** | Auto (configure.sh) | Auto (configure_cmake.bat) |
| **Executable** | `SDFGen` | `SDFGen.exe` |
| **Line Endings** | LF (`\n`) | CRLF (`\r\n`) |

## Cross-Platform Development Tips

### Working on Both Platforms

**1. Use consistent line endings:**
```bash
# .gitattributes
*.cpp text eol=lf
*.h text eol=lf
*.sh text eol=lf
*.bat text eol=crlf
```

**2. Test on both platforms:**
```bash
# Linux
./tools/configure.sh Release && ./tools/build.sh

# Windows (from Git Bash)
./tools/configure_cmake.bat Release && ./tools/build_with_vs.bat all
```

**3. Script permissions (Linux):**
```bash
# After git clone on Linux, always:
chmod +x tools/*.sh
```

### Sharing Build Between Platforms (Don't!)

**✗ Wrong:**
```
/mnt/c/Apps/Project/  ← WSL accessing Windows filesystem
```
Problems:
- Very slow I/O
- Permission issues
- Line ending conflicts

**✓ Correct:**
```
Linux:   ~/project/        (native Linux filesystem)
Windows: C:\Apps\project\  (native Windows filesystem)
```

Each platform gets its own copy and build directory.

## Summary - Linux Build

### Quick Commands

```bash
# First time setup
sudo apt-get install build-essential cmake ninja-build
cd tools
chmod +x *.sh
./configure.sh Release
./build.sh

# Daily development
./build.sh
./build-Release/bin/SDFGen test.stl 128

# After changing CMakeLists.txt
./configure.sh Release
./build.sh
```

### Key Differences from Windows

1. **Scripts end in `.sh` not `.bat`**
2. **Must be executable: `chmod +x`**
3. **Use GCC instead of MSVC**
4. **Ninja often faster than Make**
5. **CUDA detection is automatic (like Windows)**
6. **Runtime GPU detection works the same**

### GPU Acceleration on Linux

**Just like Windows:**
- No `--gpu` flag needed
- Auto-detected at build time (configure.sh)
- Auto-selected at runtime (if GPU available)
- Same PyTorch-style simplicity

```bash
# This works automatically:
./SDFGen mesh.stl 256

# Output shows:
Hardware: GPU acceleration available
Implementation: GPU (CUDA)
```

---

**End of Linux Build Guide**
