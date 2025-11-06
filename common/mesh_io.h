// SDFGen - Signed Distance Field Generator
// Copyright (c) 2015 Christopher Batty, 2025 Brad Chamberlain
// Licensed under the MIT License - see LICENSE file

#pragma once

#include "vec.h"
#include <vector>
#include <string>

// Mesh I/O library for SDFGen
// Supports OBJ and STL (both binary and ASCII) formats

namespace meshio {

// ============================================================================
// Public API
// ============================================================================

// Load Wavefront OBJ file
// Returns true on success, false on failure
// Vertex normals (vn) are rejected with an error
bool load_obj(const char* filename,
              std::vector<Vec3f>& vertList,
              std::vector<Vec3ui>& faceList,
              Vec3f& min_box,
              Vec3f& max_box);

// Load STL file (auto-detects binary vs ASCII format)
// Returns true on success, false on failure
bool load_stl(const char* filename,
              std::vector<Vec3f>& vertList,
              std::vector<Vec3ui>& faceList,
              Vec3f& min_box,
              Vec3f& max_box);

// Load mesh file (auto-detects format from extension)
// Supported: .obj, .stl
// Returns true on success, false on failure
bool load_mesh(const char* filename,
               std::vector<Vec3f>& vertList,
               std::vector<Vec3ui>& faceList,
               Vec3f& min_box,
               Vec3f& max_box);

// ============================================================================
// Utility Functions
// ============================================================================

// Update bounding box with a point
inline void update_minmax(const Vec3f& point, Vec3f& min_box, Vec3f& max_box) {
    min_box[0] = std::min(min_box[0], point[0]);
    min_box[1] = std::min(min_box[1], point[1]);
    min_box[2] = std::min(min_box[2], point[2]);
    max_box[0] = std::max(max_box[0], point[0]);
    max_box[1] = std::max(max_box[1], point[1]);
    max_box[2] = std::max(max_box[2], point[2]);
}

// Get file extension (lowercase)
std::string get_extension(const std::string& filename);

} // namespace meshio
