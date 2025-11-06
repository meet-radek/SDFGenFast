// SDFGen - Signed Distance Field Generator
// Copyright (c) 2015 Christopher Batty, 2025 Brad Chamberlain
// Licensed under the MIT License - see LICENSE file

#pragma once

#include <string>
#include <vector>
#include <cstdint>

// CLI testing utilities for spawning SDFGen executable and validating results
// Provides cross-platform process execution and output validation

namespace cli_test {

// Result from running a CLI command
struct CommandResult {
    int32_t exit_code = -1;          // Process exit code
    std::string stdout_output;       // Captured stdout
    std::string stderr_output;       // Captured stderr (if captured separately)
    bool timed_out = false;          // Whether command exceeded timeout
    bool execution_failed = false;   // Whether execution itself failed
};

// Configuration for CLI test execution
struct TestConfig {
    std::string sdfgen_exe_path;     // Path to SDFGen executable
    std::string test_resources_dir;  // Path to test resources directory
    int32_t timeout_seconds = 120;   // Maximum execution time
    bool verbose = false;            // Print command output during execution
};

// Execute SDFGen with given arguments
// Returns CommandResult with exit code and captured output
CommandResult run_sdfgen(
    const std::vector<std::string>& args,
    const TestConfig& config
);

// Helper: Build command line from arguments
std::string build_command_line(
    const std::string& executable,
    const std::vector<std::string>& args
);

// File validation helpers
bool file_exists(const std::string& path);
bool file_is_readable(const std::string& path);
int64_t get_file_size(const std::string& path);
bool delete_file_if_exists(const std::string& path);

// SDF file validation
struct SDFFileInfo {
    bool valid = false;
    int32_t nx = 0, ny = 0, nz = 0;
    float min_x = 0.0f, min_y = 0.0f, min_z = 0.0f;
    float max_x = 0.0f, max_y = 0.0f, max_z = 0.0f;
    int64_t file_size = 0;
    int64_t expected_size = 0;  // 36 + nx*ny*nz*4
};

// Read and validate SDF file header
SDFFileInfo read_sdf_header(const std::string& path);

// Get default test configuration (finds executable and resources automatically)
TestConfig get_default_test_config();

// String helpers for test assertions
bool string_contains(const std::string& haystack, const std::string& needle);
bool string_starts_with(const std::string& str, const std::string& prefix);
bool string_ends_with(const std::string& str, const std::string& suffix);

// Test assertion helpers with descriptive messages
void assert_exit_code(
    const CommandResult& result,
    int32_t expected_code,
    const std::string& test_name
);

void assert_file_exists(
    const std::string& path,
    const std::string& test_name
);

void assert_output_contains(
    const CommandResult& result,
    const std::string& expected_text,
    const std::string& test_name
);

void assert_sdf_dimensions(
    const SDFFileInfo& info,
    int32_t expected_nx,
    int32_t expected_ny,
    int32_t expected_nz,
    const std::string& test_name
);

} // namespace cli_test
