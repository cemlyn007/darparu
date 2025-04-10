#pragma once
#include <deque>
#include <iostream>
#include <limits>
#include <vector>

// Kudos to Claude 3.7 Sonnet thinking and whoever fed the beast.
std::vector<std::vector<double>> triangulate_polygon(const std::vector<double> &vertices) {
  // Result will contain all triangles as [x1,y1, x2,y2, x3,y3] triplets
  std::vector<std::vector<double>> triangles;

  // Need at least 3 vertices to form a triangle
  const size_t vertex_count = vertices.size() / 2;
  if (vertex_count < 3)
    return triangles;

  // Special case: if we already have a triangle, return it directly
  if (vertex_count == 3) {
    triangles.push_back(vertices);
    return triangles;
  }

  // Create a deque containing vertex indices for easier manipulation
  std::deque<int> remaining;
  for (size_t i = 0; i < vertex_count; i++) {
    remaining.push_back(i);
  }

  // Determine polygon orientation (CCW is positive area)
  double signed_area = 0.0f;
  for (size_t i = 0; i < vertex_count; i++) {
    const size_t j = (i + 1) % vertex_count;
    signed_area += vertices[2 * i] * vertices[2 * j + 1] - vertices[2 * j] * vertices[2 * i + 1];
  }
  const bool is_ccw = (signed_area > 0.0f);

  // Helper function to compute the cross product (z component) of vectors (p2-p1) and (p3-p1)
  auto cross_product = [&](int i1, int i2, int i3) -> double {
    const double x1 = vertices[2 * i1], y1 = vertices[2 * i1 + 1];
    const double x2 = vertices[2 * i2], y2 = vertices[2 * i2 + 1];
    const double x3 = vertices[2 * i3], y3 = vertices[2 * i3 + 1];
    return (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
  };

  // Helper function to check if a vertex is convex
  auto is_vertex_convex = [&](int prev, int curr, int next) -> bool {
    const double cross = cross_product(prev, curr, next);
    return is_ccw ? (cross > 0) : (cross < 0);
  };

  // Helper function to check if point p is inside the triangle formed by a,b,c
  auto is_point_in_triangle = [&](int p, int a, int b, int c) -> bool {
    // Skip if the test point is one of the triangle vertices
    if (p == a || p == b || p == c)
      return false;

    // Use a more robust approach: check if point is on the same side of all three edges
    const double x = vertices[2 * p], y = vertices[2 * p + 1];
    const double x1 = vertices[2 * a], y1 = vertices[2 * a + 1];
    const double x2 = vertices[2 * b], y2 = vertices[2 * b + 1];
    const double x3 = vertices[2 * c], y3 = vertices[2 * c + 1];

    // Compute edge vectors and point-to-vertex vectors
    auto side_test = [](double px, double py, double x1, double y1, double x2, double y2) -> double {
      return (px - x2) * (y1 - y2) - (x1 - x2) * (py - y2);
    };

    // Check if point is on the same side of all edges
    double s1 = side_test(x, y, x1, y1, x2, y2);
    double s2 = side_test(x, y, x2, y2, x3, y3);
    double s3 = side_test(x, y, x3, y3, x1, y1);

    // If signs are all the same, point is inside
    const double epsilon = 1e-6f;

    // Handle the case where point is exactly on an edge
    if (std::abs(s1) < epsilon || std::abs(s2) < epsilon || std::abs(s3) < epsilon)
      return false; // Don't consider points on edges as "inside"

    return (s1 > 0 && s2 > 0 && s3 > 0) || (s1 < 0 && s2 < 0 && s3 < 0);
  };

  // Prevent infinite loops
  const size_t max_iterations = vertex_count * 2;
  size_t iterations = 0;
  size_t fail_safe_counter = 0; // Additional counter to prevent getting stuck

  // Pre-check for degenerate cases
  bool has_degeneracies = false;
  for (size_t i = 0; i < vertex_count; i++) {
    size_t next = (i + 1) % vertex_count;
    double dx = vertices[2 * i] - vertices[2 * next];
    double dy = vertices[2 * i + 1] - vertices[2 * next + 1];
    if (std::abs(dx) < 1e-6f && std::abs(dy) < 1e-6f) {
      has_degeneracies = true;
      break;
    }
  }

  if (has_degeneracies) {
    std::cerr << "Warning: Degenerate polygon detected (repeated vertices)" << std::endl;
  }

  // Ear clipping algorithm
  while (remaining.size() > 3 && iterations < max_iterations) {
    iterations++;
    bool ear_found = false;

    // If we've tried too many times without progress, use a fallback approach
    if (fail_safe_counter > remaining.size() * 2) {
      std::cerr << "Warning: Using fallback triangulation for a difficult region" << std::endl;
      // Simple fan triangulation as fallback
      if (remaining.size() >= 3) {
        int anchor = remaining.front();
        for (size_t i = 1; i < remaining.size() - 1; i++) {
          triangles.push_back({vertices[2 * anchor], vertices[2 * anchor + 1], vertices[2 * remaining[i]],
                               vertices[2 * remaining[i] + 1], vertices[2 * remaining[i + 1]],
                               vertices[2 * remaining[i + 1] + 1]});
        }
      }
      break;
    }

    // Try to find an ear in the current polygon
    for (size_t i = 0; i < remaining.size(); i++) {
      // Get three consecutive vertices
      const size_t prev_idx = (i + remaining.size() - 1) % remaining.size();
      const size_t curr_idx = i;
      const size_t next_idx = (i + 1) % remaining.size();

      const int prev = remaining[prev_idx];
      const int curr = remaining[curr_idx];
      const int next = remaining[next_idx];

      // Check if the vertex is convex
      if (!is_vertex_convex(prev, curr, next)) {
        continue;
      }

      // Check if this is an ear (no other vertex inside the triangle)
      bool is_ear = true;
      for (size_t j = 0; j < remaining.size(); j++) {
        if (j == prev_idx || j == curr_idx || j == next_idx)
          continue;

        if (is_point_in_triangle(remaining[j], prev, curr, next)) {
          is_ear = false;
          break;
        }
      }

      if (is_ear) {
        // We found an ear, add the triangle to our list
        triangles.push_back({vertices[2 * prev], vertices[2 * prev + 1], vertices[2 * curr], vertices[2 * curr + 1],
                             vertices[2 * next], vertices[2 * next + 1]});

        // Remove the ear vertex
        remaining.erase(remaining.begin() + curr_idx);
        ear_found = true;
        fail_safe_counter = 0; // Reset the fail safe counter
        break;
      }
    }

    // No ear found, this may happen with degenerate polygons
    if (!ear_found) {
      fail_safe_counter++; // Increment failsafe to eventually trigger fallback

      // Handle the case where no ear is initially found by relaxing constraints
      if (fail_safe_counter > remaining.size()) {
        // Find the most convex vertex and clip it anyway
        double best_convexity = -std::numeric_limits<double>::max();
        size_t best_idx = 0;

        for (size_t i = 0; i < remaining.size(); i++) {
          const size_t prev_idx = (i + remaining.size() - 1) % remaining.size();
          const size_t next_idx = (i + 1) % remaining.size();

          const int prev = remaining[prev_idx];
          const int curr = remaining[i];
          const int next = remaining[next_idx];

          double convexity = cross_product(prev, curr, next);
          if (is_ccw)
            convexity = -convexity; // Adjust for orientation

          if (convexity > best_convexity) {
            best_convexity = convexity;
            best_idx = i;
          }
        }

        // Create a triangle with the best vertex
        const size_t prev_idx = (best_idx + remaining.size() - 1) % remaining.size();
        const size_t next_idx = (best_idx + 1) % remaining.size();

        triangles.push_back({vertices[2 * remaining[prev_idx]], vertices[2 * remaining[prev_idx] + 1],
                             vertices[2 * remaining[best_idx]], vertices[2 * remaining[best_idx] + 1],
                             vertices[2 * remaining[next_idx]], vertices[2 * remaining[next_idx] + 1]});

        remaining.erase(remaining.begin() + best_idx);
        ear_found = true;
        fail_safe_counter = 0;
      }
    }
  }

  // Add the final triangle
  if (remaining.size() == 3) {
    int a = remaining[0], b = remaining[1], c = remaining[2];
    triangles.push_back({vertices[2 * a], vertices[2 * a + 1], vertices[2 * b], vertices[2 * b + 1], vertices[2 * c],
                         vertices[2 * c + 1]});
  }

  return triangles;
}