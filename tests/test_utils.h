// SDFGen - Signed Distance Field Generator
// Copyright (c) 2015 Christopher Batty, 2025 Brad Chamberlain
// Licensed under the MIT License - see LICENSE file

#pragma once

#include "sdfgen_unified.h"
#include "sdf_io.h"
#include <string>
#include <cstdint>

// Test utilities for SDFGen test suite
// Provides common functions to eliminate code duplication across tests

namespace test_utils {

// Constants
constexpr float BBOX_TOLERANCE = 1e-5f;           // Tolerance for bounding box comparisons
constexpr float MAX_DIFF_THRESHOLD = 25.0f;       // Maximum acceptable difference in cell widths
constexpr int32_t MAX_MISMATCH_PRINT = 5;         // Number of mismatches to print

// Test result structure
struct SDFComparisonResult {
    bool dimensions_match = false;
    bool bbox_match = false;
    int32_t total_cells = 0;
    int32_t mismatch_count = 0;
    float max_diff = 0.0f;
    float tolerance = 0.0f;

    double cpu_time_ms = 0.0;
    double gpu_time_ms = 0.0;
    int32_t cpu_inside_count = 0;
    int32_t gpu_inside_count = 0;

    bool passed() const {
        return dimensions_match && bbox_match &&
               (max_diff / tolerance) < (MAX_DIFF_THRESHOLD * 2.0f);
    }
};

// Generate SDF with timing measurement
void generate_sdf_with_timing(
    const std::vector<Vec3ui>& faceList,
    const std::vector<Vec3f>& vertList,
    const Vec3f& origin,
    float dx,
    int32_t nx, int32_t ny, int32_t nz,
    Array3f& phi,
    sdfgen::HardwareBackend backend,
    double& time_ms
);

// Write SDF to file and measure inside cell count
bool write_sdf_with_validation(
    const char* filename,
    const Array3f& phi,
    const Vec3f& origin,
    float dx,
    int32_t& inside_count
);

// Compare two SDF grids with detailed reporting
SDFComparisonResult compare_sdf_grids(
    const Array3f& phi_cpu,
    const Array3f& phi_gpu,
    const Vec3f& cpu_origin,
    const Vec3f& gpu_origin,
    const Vec3f& expected_origin,
    float dx,
    bool verbose = true
);

// Test write/read roundtrip for both CPU and GPU
bool test_sdf_io_roundtrip(
    const std::vector<Vec3ui>& faceList,
    const std::vector<Vec3f>& vertList,
    const Vec3f& origin,
    float dx,
    int32_t nx, int32_t ny, int32_t nz,
    const char* cpu_filename,
    const char* gpu_filename,
    SDFComparisonResult& result
);

// Print test summary
void print_test_summary(
    const char* test_name,
    const SDFComparisonResult& result
);

// Print mesh info
void print_mesh_info(
    const std::vector<Vec3f>& vertList,
    const std::vector<Vec3ui>& faceList,
    const Vec3f& min_box,
    const Vec3f& max_box
);

// Calculate grid parameters from mesh bounds
void calculate_grid_parameters(
    const Vec3f& min_box,
    const Vec3f& max_box,
    int32_t target_nx,
    int32_t padding,
    float& dx,
    int32_t& ny,
    int32_t& nz,
    Vec3f& origin
);

} // namespace test_utils
