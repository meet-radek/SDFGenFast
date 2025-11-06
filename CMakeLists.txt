cmake_minimum_required(VERSION 3.20)

project(SDFGen VERSION 2.0 LANGUAGES CXX)

# ============================================================================
# Package Discovery - Auto-detect CUDA
# ============================================================================

# Try to find CUDA quietly first (for auto-detection)
find_package(CUDAToolkit QUIET)
if(CUDAToolkit_FOUND)
    message(STATUS "SDFGen: CUDA Toolkit found - GPU support available")
    # Enable CUDA language
    enable_language(CUDA)
    # Set default GPU build option to ON
    option(SDFGEN_BUILD_GPU "Build GPU-accelerated implementation" ON)
else()
    message(STATUS "SDFGen: CUDA Toolkit not found - building CPU-only")
    # Force GPU build option to OFF
    set(SDFGEN_BUILD_GPU OFF CACHE BOOL "Build GPU-accelerated implementation" FORCE)
endif()

# ============================================================================
# Build Options
# ============================================================================
# Note: SDFGEN_BUILD_GPU is set above based on CUDA availability
# Users can still override with: cmake -DSDFGEN_BUILD_GPU=OFF

# ============================================================================
# Package Discovery - VTK (optional)
# ============================================================================

# VTK (optional, for .vti output format)
find_package(VTK QUIET)
if(VTK_FOUND)
    set(HAVE_VTK 1)
    message(STATUS "SDFGen: VTK found (${VTK_VERSION}) - .vti output enabled")
else()
    set(HAVE_VTK 0)
    message(STATUS "SDFGen: Building without VTK - binary .sdf output only")
endif()

# Set HAVE_CUDA based on final SDFGEN_BUILD_GPU value
if(SDFGEN_BUILD_GPU)
    set(HAVE_CUDA 1)
    message(STATUS "SDFGen: GPU acceleration enabled")
    message(STATUS "SDFGen: CUDA Compiler: ${CMAKE_CUDA_COMPILER}")
else()
    set(HAVE_CUDA 0)
    message(STATUS "SDFGen: GPU build disabled")
endif()

# Generate config.h from config.h.in
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

# Make generated config.h available to all subdirectories
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# ============================================================================
# Output Directories
# ============================================================================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

# ============================================================================
# Subdirectories
# ============================================================================

# Always build common and CPU libraries
add_subdirectory(common)
add_subdirectory(cpu_lib)

# Conditionally build GPU library
if(HAVE_CUDA AND SDFGEN_BUILD_GPU)
    add_subdirectory(gpu_lib)
endif()

# Always build the main application
add_subdirectory(app)

# Conditionally build tests (only if GPU is built)
if(HAVE_CUDA AND SDFGEN_BUILD_GPU)
    enable_testing()
    add_subdirectory(tests)
endif()

# ============================================================================
# Installation
# ============================================================================
install(TARGETS SDFGen
    RUNTIME DESTINATION bin
)

# ============================================================================
# Build Summary
# ============================================================================
message(STATUS "")
message(STATUS "========================================")
message(STATUS "SDFGen Configuration Summary")
message(STATUS "========================================")
message(STATUS "  Version:        ${PROJECT_VERSION}")
message(STATUS "  C++ Compiler:   ${CMAKE_CXX_COMPILER}")
message(STATUS "  Build Type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "  VTK Support:    ${HAVE_VTK}")
message(STATUS "  CUDA Support:   ${HAVE_CUDA}")
if(HAVE_CUDA)
message(STATUS "  CUDA Compiler:  ${CMAKE_CUDA_COMPILER}")
endif()
message(STATUS "========================================")
message(STATUS "")
