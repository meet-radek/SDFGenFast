# SDFGen

A high-performance command-line utility for generating grid-based signed distance fields (SDFs) from triangle meshes.

**Enhanced fork** of [Christopher Batty's original SDFGen](https://github.com/christopherbatty/SDFGen) with GPU acceleration, automatic hardware detection, and cross-platform build system.

## Features

- **Automatic GPU Acceleration**: Detects and uses CUDA-capable GPUs automatically (no manual flags needed)
- **Fast CPU Fallback**: Multi-threaded CPU implementation when GPU is unavailable
- **Multiple Input Formats**: Binary/ASCII STL and Wavefront OBJ (quads automatically triangulated)
- **Flexible Grid Sizing**: Proportional or manual dimension specification
- **Binary SDF Output**: Compact binary format with metadata header
- **Cross-Platform**: Builds on Windows (MSVC) and Linux (GCC)

## What's New in This Fork

This enhanced version adds significant improvements over the [original SDFGen](https://github.com/christopherbatty/SDFGen):

### Performance Enhancements
- **Multi-threaded CPU**: Parallel CPU implementation (6-10x faster than single-threaded, configurable thread count)
- **GPU Acceleration**: CUDA-based GPU implementation with additional 1.1-5.4x speedup over multi-threaded CPU
- **Automatic Backend Selection**: PyTorch-style automatic GPU detection (no manual flags!)
- **Fast Sweeping Algorithm**: Optimized GPU implementation of Eikonal solver

### New Features
- **STL Support**: Binary and ASCII STL file formats (original only supported OBJ)
- **Flexible Grid Modes**: Proportional dimension calculation for STL files
- **Improved CLI**: Multiple usage modes with better parameter handling
- **Thread Control**: Configurable CPU thread count for optimal performance
- **Comprehensive Testing**: 14-test suite covering all functionality

### Build System
- **Cross-Platform**: Automated build scripts for Windows (MSVC) and Linux (GCC/Clang)
- **CMake 3.20+**: Modern CMake with automatic CUDA detection
- **Professional Tooling**: Complete with configuration scripts, test suite, and documentation

### Developer Experience
- **Auto-Detection**: Build-time CUDA detection, runtime GPU detection
- **Better Errors**: Clear error messages with installation instructions
- **Documentation**: Comprehensive build guide, README, and inline docs

**Performance:** Multi-threaded CPU implementation provides 5.8-11.6x speedup over single-threaded (optimal at 10-20 threads). GPU provides additional 1.1-5.4x speedup over multi-threaded CPU, scaling with grid size (thread count configurable, auto-detects by default).

### Comparison with Original

| Feature | Original SDFGen | This Fork |
|---------|----------------|-----------|
| **Performance** | Single-threaded CPU | Multi-threaded CPU + GPU (CUDA) |
| **Input Formats** | OBJ only | OBJ + Binary/ASCII STL |
| **Grid Sizing** | Manual dx calculation | Auto-proportional + manual modes |
| **Build System** | Basic Makefile | CMake 3.20+ with auto-detection |
| **Platforms** | Linux/Mac | Windows + Linux with automated scripts |
| **GPU Support** | None | Automatic detection and usage |
| **Testing** | None | 14-test comprehensive suite |
| **Documentation** | Basic | Complete guide + examples |
| **Speed (256³ grid)** | ~20 seconds | 48.6s (1 thread), 4.18s (20 threads), 1.29s GPU |

## Hardware Acceleration

SDFGen automatically detects and uses GPU acceleration at both **build-time** and **runtime**:

### Build-Time Detection
```bash
# Linux
./tools/configure.sh Release
./tools/build.sh

# Windows
tools\configure_cmake.bat Release
tools\build_with_vs.bat all
```

If CUDA Toolkit is found, GPU support is automatically compiled in. No manual configuration needed.

### Runtime Detection
```bash
# Same command works everywhere - automatically uses GPU if available
./build-Release/bin/SDFGen mesh.stl 128
```

Output shows what hardware is being used:
```
Hardware: GPU acceleration available
Implementation: GPU (CUDA)
```

or

```
Hardware: No CUDA GPU detected
Implementation: CPU (multi-threaded)
```

**PyTorch-style simplicity**: No `--gpu` flags, no configuration files. It just works.

## Supported Platforms

| Platform | Compiler | Build System | Status |
|----------|----------|--------------|--------|
| Windows 10/11 | MSVC 2022 | CMake + Ninja/MSBuild | ✓ Tested |
| Ubuntu 20.04 LTS | GCC 9+ | CMake + Ninja/Make | ✓ Supported |
| Ubuntu 22.04 LTS | GCC 11+ | CMake + Ninja/Make | ✓ Supported |

**Requirements:**
- CMake 3.20+
- C++17 compiler
- CUDA Toolkit 11.0+ (optional, for GPU acceleration)
- VTK 9.0+ (optional, for .vti output)

## Quick Start

### Windows

```powershell
# 1. Configure (auto-detects CUDA)
cd tools
.\configure_cmake.bat Release

# 2. Build
.\build_with_vs.bat all

# 3. Run
cd ..
.\build-Release\bin\SDFGen.exe mesh.stl 128
```

### Linux

```bash
# 1. Configure (auto-detects CUDA)
cd tools
chmod +x configure.sh build.sh
./configure.sh Release

# 2. Build
./build.sh

# 3. Run
./build-Release/bin/SDFGen mesh.stl 128
```

## Usage

### Mode 1: OBJ with Cell Size (dx)

```bash
SDFGen <file.obj> <dx> <padding> [threads]
```

**Example:**
```bash
SDFGen bunny.obj 0.01 2
# Output: bunny.sdf

# With custom thread count
SDFGen bunny.obj 0.01 2 10
# Uses 10 CPU threads
```

Where:
- `dx`: Grid cell size (determines resolution automatically)
- `padding`: Number of padding cells around mesh (minimum 1)
- `threads`: Optional CPU thread count (0=auto-detect, default: 0)

### Mode 2a: STL with Proportional Dimensions (Recommended)

```bash
SDFGen <file.stl> <Nx> [padding] [threads]
```

**Example:**
```bash
SDFGen dragon.stl 256 1
# Output: dragon_sdf_256x342x189.sdf (dimensions calculated proportionally)

# With custom thread count
SDFGen dragon.stl 256 1 10
# Uses 10 CPU threads
```

Where:
- `Nx`: Grid size in X dimension (Y and Z calculated to preserve aspect ratio)
- `padding`: Optional padding cells (default: 1)
- `threads`: Optional CPU thread count (0=auto-detect, default: 0)

### Mode 2b: STL with Manual Dimensions

```bash
SDFGen <file.stl> <Nx> <Ny> <Nz> [padding] [threads]
```

**Example:**
```bash
SDFGen teapot.stl 128 128 128 2
# Output: teapot_sdf_128x128x128.sdf

# With custom thread count
SDFGen teapot.stl 128 128 128 2 10
# Uses 10 CPU threads
```

Where:
- `Nx`, `Ny`, `Nz`: Exact grid dimensions
- `padding`: Optional padding cells (default: 1)
- `threads`: Optional CPU thread count (0=auto-detect, default: 0)

## Performance

Comprehensive benchmark showing multi-threading scaling and GPU acceleration:

| Grid Size | Total Cells | CPU (1 thread) | CPU (10 threads) | CPU (20 threads) | GPU      |
|-----------|-------------|----------------|------------------|------------------|----------|
| 64³       | 559K        | 738 ms         | 127 ms           | 93 ms            | 111 ms   |
| 128³      | 4.6M        | 5.98 s         | 789 ms           | 748 ms           | 358 ms   |
| 256³      | 36.9M       | 48.6 s         | 6.95 s           | 4.18 s           | 1.29 s   |

*Benchmarked on Intel i9-13900K + NVIDIA RTX 4090*

**Multi-threading Efficiency:**
- 10 threads: 5.8-7.6x speedup (58-76% efficiency)
- 20 threads: 7.9-11.6x speedup (40-58% efficiency)

**GPU Acceleration:**
- Small grids (64³): 6.7x faster than 1 thread, similar to 20 threads
- Medium grids (128³): 16.7x faster than 1 thread, 2.1x faster than 20-thread CPU
- Large grids (256³): 37.6x faster than 1 thread, 3.2x faster than 20-thread CPU

**Key insights:**
- Multi-threading scales well up to 10 threads (58-76% efficiency)
- Efficiency drops to 40-58% at 20 threads due to memory bandwidth saturation
- GPU advantage increases dramatically with grid size
- Best performance: GPU for large grids, 10-20 threads for smaller grids

**Thread Configuration:**
- Default behavior: Auto-detects CPU core count
- Custom configuration: Add optional `threads` parameter to any command
- Examples: `SDFGen mesh.obj 0.1 2 10` (use 10 threads), `SDFGen mesh.stl 128 1 20` (use 20 threads)
- Use `0` for auto-detection: `SDFGen mesh.obj 0.1 2 0`

Run the benchmark yourself:
```bash
cd tests
../build-Release/bin/benchmark_performance
```

## Building from Source

### Windows Prerequisites

Install:
1. [Visual Studio 2022](https://visualstudio.microsoft.com/) (Community edition works)
2. [CMake 3.20+](https://cmake.org/download/)
3. [CUDA Toolkit 11.0+](https://developer.nvidia.com/cuda-downloads) (optional, for GPU support)
4. [Ninja](https://github.com/ninja-build/ninja/releases) (optional, for faster builds)

### Linux Prerequisites

**Ubuntu/Debian:**
```bash
# Essential tools
sudo apt-get update
sudo apt-get install build-essential cmake git

# Optional: Ninja for faster builds
sudo apt-get install ninja-build

# Optional: CUDA for GPU acceleration
# Follow: https://developer.nvidia.com/cuda-downloads

# Optional: VTK for .vti output
sudo apt-get install libvtk9-dev
```

### Build Instructions

See [tools/BUILD_GUIDE.md](tools/BUILD_GUIDE.md) for detailed build instructions.

## Output Format

Binary SDF file format:

**Header (36 bytes):**
```
[Nx, Ny, Nz]           (3 x int32)   - Grid dimensions
[xmin, ymin, zmin]     (3 x float32) - Bounding box minimum
[xmax, ymax, zmax]     (3 x float32) - Bounding box maximum
```

**Data (Nx × Ny × Nz × 4 bytes):**
```
float32 distance values in Z-major order
```

**Distance convention:**
- Negative values: Inside the mesh
- Positive values: Outside the mesh
- Zero: On the surface

## Testing

SDFGen includes comprehensive test suite (14 tests):

**Test Categories:**
- **Correctness Tests** (3): `test_correctness`, `test_file_io`, `test_mode1_legacy`
- **CLI Integration Tests** (6): `test_cli_modes`, `test_cli_backend`, `test_cli_formats`, `test_cli_output`, `test_cli_errors`, `test_cli_threads`
- **File Format Tests** (3): `test_stl_file_io`, `test_obj_file_io`, `test_ascii_stl`
- **Edge Case Tests** (2): `test_thread_slice_ratios`, `test_vtk_output`

### Windows
```powershell
# Run all tests
cd tools
.\build_with_vs.bat test

# Run specific test
cd ..\tests
..\build-Release\bin\test_cli_backend.exe
```

### Linux
```bash
# Run all tests
cd build-Release
ctest --output-on-failure

# Run specific test
cd ../tests
../build-Release/bin/test_cli_backend
```

## Project Structure

```
SDFGenFast/
├── app/              # Main application (CLI)
├── common/           # Shared utilities (unified API, I/O)
├── cpu_lib/          # CPU implementation (multi-threaded)
├── gpu_lib/          # GPU implementation (CUDA)
├── tests/            # Test suite (12 tests)
├── tools/            # Build scripts (Windows + Linux)
│   ├── build_with_vs.bat     # Windows build
│   ├── configure_cmake.bat   # Windows configuration
│   ├── build.sh              # Linux build
│   └── configure.sh          # Linux configuration
└── CMakeLists.txt    # Root build configuration
```

## Contributing

Contributions are welcome! Areas for improvement:

- [ ] OpenCL backend for AMD GPUs
- [ ] Metal backend for Apple Silicon
- [ ] Multi-GPU support
- [ ] Mesh preprocessing (repair, smoothing)
- [ ] Additional output formats (VDB, raw)

## History and Attribution

This project builds upon excellent foundational work:

1. **Robert Bridson** - Original level set algorithms and code
   [https://www.cs.ubc.ca/~rbridson](https://www.cs.ubc.ca/~rbridson)

2. **Christopher Batty** (2015) - Original SDFGen implementation
   [https://github.com/christopherbatty/SDFGen](https://github.com/christopherbatty/SDFGen)
   First unified, easy-to-use SDF generator with OBJ support

3. **Brad Chamberlain** (2025) - This enhanced fork
   GPU acceleration, auto-detection, STL support, cross-platform builds, and comprehensive test suite

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```
Copyright (c) 2015 Christopher Batty (Original SDFGen)
Copyright (c) 2025 Brad Chamberlain (GPU acceleration and enhancements)
```

Both the original work and all enhancements are freely available under the MIT License.

## Citation

If you use this software in academic work, please cite:

**For GPU-accelerated version:**
```
Brad Chamberlain (2025). SDFGen: GPU-Accelerated Signed Distance Field Generator.
Fork of Christopher Batty's SDFGen with CUDA acceleration and automatic hardware detection.
```

**For original algorithm:**
```
Christopher Batty (2015). SDFGen: A simple grid-based signed distance field generator.
Based on code by Robert Bridson.
```

## Acknowledgments

- **Robert Bridson**: Original level set algorithms and numerical methods
- **Christopher Batty**: Original SDFGen implementation and API design
- **Brad Chamberlain**: GPU acceleration, auto-detection, cross-platform build system
- **NVIDIA**: CUDA toolkit and GPU computing framework
