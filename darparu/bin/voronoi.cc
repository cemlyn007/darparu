#include "darparu/renderer/algebra.h"
#include "darparu/renderer/cameras/pan.h"
#include "darparu/renderer/entities/mesh_2d.h"
#include "darparu/renderer/io_controls/simple_2d.h"
#include "darparu/renderer/projection_context.h"
#include "darparu/renderer/renderer.h"
#include "math.h"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <deque>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std::chrono_literals;

using namespace darparu;

std::vector<std::vector<double>> load_voronoi_faces(const std::string &file_path) {
  std::ifstream file(file_path);
  if (!file.is_open()) {
    std::cerr << "Error opening file" << std::endl;
    return {};
  }
  std::vector<std::vector<double>> data;
  std::string line;
  // Skip the header line
  std::getline(file, line);
  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string cell;
    std::vector<double> row;
    while (std::getline(ss, cell, ',')) {
      cell.erase(remove(cell.begin(), cell.end(), '"'), cell.end());
      row.push_back(std::stod(cell));
    }
    data.push_back(row);
  }
  file.close();
  return data;
}

// Kudos to Claude 3.7 Sonnet thinking and whoever fed the beast.
std::vector<std::vector<double>> triangulate_polygon(const std::vector<double> &vertices) {
  // Result will contain all triangles as [x1,y1, x2,y2, x3,y3] triplets
  std::vector<std::vector<double>> triangles;

  // Need at least 3 vertices to form a triangle
  const size_t vertexCount = vertices.size() / 2;
  if (vertexCount < 3)
    return triangles;

  // Special case: if we already have a triangle, return it directly
  if (vertexCount == 3) {
    triangles.push_back(vertices);
    return triangles;
  }

  // Create a deque containing vertex indices for easier manipulation
  std::deque<int> remaining;
  for (size_t i = 0; i < vertexCount; i++) {
    remaining.push_back(i);
  }

  // Determine polygon orientation (CCW is positive area)
  double signedArea = 0.0f;
  for (size_t i = 0; i < vertexCount; i++) {
    const size_t j = (i + 1) % vertexCount;
    signedArea += vertices[2 * i] * vertices[2 * j + 1] - vertices[2 * j] * vertices[2 * i + 1];
  }
  const bool isCCW = (signedArea > 0.0f);

  // Helper function to compute the cross product (z component) of vectors (p2-p1) and (p3-p1)
  auto crossProduct = [&](int i1, int i2, int i3) -> double {
    const double x1 = vertices[2 * i1], y1 = vertices[2 * i1 + 1];
    const double x2 = vertices[2 * i2], y2 = vertices[2 * i2 + 1];
    const double x3 = vertices[2 * i3], y3 = vertices[2 * i3 + 1];
    return (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
  };

  // Helper function to check if a vertex is convex
  auto isVertexConvex = [&](int prev, int curr, int next) -> bool {
    const double cross = crossProduct(prev, curr, next);
    return isCCW ? (cross > 0) : (cross < 0);
  };

  // Helper function to check if point p is inside the triangle formed by a,b,c
  auto isPointInTriangle = [&](int p, int a, int b, int c) -> bool {
    // Skip if the test point is one of the triangle vertices
    if (p == a || p == b || p == c)
      return false;

    // Use a more robust approach: check if point is on the same side of all three edges
    const double x = vertices[2 * p], y = vertices[2 * p + 1];
    const double x1 = vertices[2 * a], y1 = vertices[2 * a + 1];
    const double x2 = vertices[2 * b], y2 = vertices[2 * b + 1];
    const double x3 = vertices[2 * c], y3 = vertices[2 * c + 1];

    // Compute edge vectors and point-to-vertex vectors
    auto sideTest = [](double px, double py, double x1, double y1, double x2, double y2) -> double {
      return (px - x2) * (y1 - y2) - (x1 - x2) * (py - y2);
    };

    // Check if point is on the same side of all edges
    double s1 = sideTest(x, y, x1, y1, x2, y2);
    double s2 = sideTest(x, y, x2, y2, x3, y3);
    double s3 = sideTest(x, y, x3, y3, x1, y1);

    // If signs are all the same, point is inside
    const double epsilon = 1e-6f;

    // Handle the case where point is exactly on an edge
    if (std::abs(s1) < epsilon || std::abs(s2) < epsilon || std::abs(s3) < epsilon)
      return false; // Don't consider points on edges as "inside"

    return (s1 > 0 && s2 > 0 && s3 > 0) || (s1 < 0 && s2 < 0 && s3 < 0);
  };

  // Prevent infinite loops
  const size_t maxIterations = vertexCount * 2;
  size_t iterations = 0;
  size_t failSafeCounter = 0; // Additional counter to prevent getting stuck

  // Pre-check for degenerate cases
  bool hasDegeneracies = false;
  for (size_t i = 0; i < vertexCount; i++) {
    size_t next = (i + 1) % vertexCount;
    double dx = vertices[2 * i] - vertices[2 * next];
    double dy = vertices[2 * i + 1] - vertices[2 * next + 1];
    if (std::abs(dx) < 1e-6f && std::abs(dy) < 1e-6f) {
      hasDegeneracies = true;
      break;
    }
  }

  if (hasDegeneracies) {
    std::cerr << "Warning: Degenerate polygon detected (repeated vertices)" << std::endl;
  }

  // Ear clipping algorithm
  while (remaining.size() > 3 && iterations < maxIterations) {
    iterations++;
    bool earFound = false;

    // If we've tried too many times without progress, use a fallback approach
    if (failSafeCounter > remaining.size() * 2) {
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
      const size_t prevIdx = (i + remaining.size() - 1) % remaining.size();
      const size_t currIdx = i;
      const size_t nextIdx = (i + 1) % remaining.size();

      const int prev = remaining[prevIdx];
      const int curr = remaining[currIdx];
      const int next = remaining[nextIdx];

      // Check if the vertex is convex
      if (!isVertexConvex(prev, curr, next)) {
        continue;
      }

      // Check if this is an ear (no other vertex inside the triangle)
      bool isEar = true;
      for (size_t j = 0; j < remaining.size(); j++) {
        if (j == prevIdx || j == currIdx || j == nextIdx)
          continue;

        if (isPointInTriangle(remaining[j], prev, curr, next)) {
          isEar = false;
          break;
        }
      }

      if (isEar) {
        // We found an ear, add the triangle to our list
        triangles.push_back({vertices[2 * prev], vertices[2 * prev + 1], vertices[2 * curr], vertices[2 * curr + 1],
                             vertices[2 * next], vertices[2 * next + 1]});

        // Remove the ear vertex
        remaining.erase(remaining.begin() + currIdx);
        earFound = true;
        failSafeCounter = 0; // Reset the fail safe counter
        break;
      }
    }

    // No ear found, this may happen with degenerate polygons
    if (!earFound) {
      failSafeCounter++; // Increment failsafe to eventually trigger fallback

      // Handle the case where no ear is initially found by relaxing constraints
      if (failSafeCounter > remaining.size()) {
        // Find the most convex vertex and clip it anyway
        double bestConvexity = -std::numeric_limits<double>::max();
        size_t bestIdx = 0;

        for (size_t i = 0; i < remaining.size(); i++) {
          const size_t prevIdx = (i + remaining.size() - 1) % remaining.size();
          const size_t nextIdx = (i + 1) % remaining.size();

          const int prev = remaining[prevIdx];
          const int curr = remaining[i];
          const int next = remaining[nextIdx];

          double convexity = crossProduct(prev, curr, next);
          if (isCCW)
            convexity = -convexity; // Adjust for orientation

          if (convexity > bestConvexity) {
            bestConvexity = convexity;
            bestIdx = i;
          }
        }

        // Create a triangle with the best vertex
        const size_t prevIdx = (bestIdx + remaining.size() - 1) % remaining.size();
        const size_t nextIdx = (bestIdx + 1) % remaining.size();

        triangles.push_back({vertices[2 * remaining[prevIdx]], vertices[2 * remaining[prevIdx] + 1],
                             vertices[2 * remaining[bestIdx]], vertices[2 * remaining[bestIdx] + 1],
                             vertices[2 * remaining[nextIdx]], vertices[2 * remaining[nextIdx] + 1]});

        remaining.erase(remaining.begin() + bestIdx);
        earFound = true;
        failSafeCounter = 0;
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

int main() {
  std::vector<std::vector<double>> voronoi = load_voronoi_faces("voronoi_faces.csv");

  std::vector<float> vertices;
  std::vector<unsigned int> indices;
  std::vector<float> colors;
  size_t num_regions = voronoi.size();

  size_t voronoi_index = 0;
  size_t current_indice_index = 0;
  while (voronoi.size() > 0) {
    std::cout << "\rProcessing regions: " << (static_cast<double>(voronoi_index) / num_regions) * 100.0 << "%";

    std::vector<double> row = voronoi.back();
    voronoi.pop_back();
    // Triangular the row and store the vertices in the vertices vector
    std::vector<std::vector<double>> triangles = triangulate_polygon(row);
    // Generate colors for each triangle based on the current region index
    float r = std::rand() % 256 / 255.0;
    float g = std::rand() % 256 / 255.0;
    float b = std::rand() % 256 / 255.0;
    for (const auto &tri : triangles) {
      vertices.insert(vertices.end(), tri.begin(), tri.end());
      // Generate indices for the triangles
      indices.push_back(current_indice_index++);
      indices.push_back(current_indice_index++);
      indices.push_back(current_indice_index++);

      colors.push_back(r);
      colors.push_back(g);
      colors.push_back(b);

      colors.push_back(r);
      colors.push_back(g);
      colors.push_back(b);

      colors.push_back(r);
      colors.push_back(g);
      colors.push_back(b);
    }
    voronoi_index++;
  }

  std::cout << "\nVertices count: " << vertices.size() << ", Colors count: " << colors.size()
            << ", Indices count: " << indices.size() << std::endl;
  std::cout << std::flush;

  renderer::init();

  renderer::Renderer renderer(
      "Darparu", 1080, 1080,
      [](const renderer::ProjectionContext &context) {
        return renderer::orthographic(-0.5f * context.zoom, 0.5f * context.zoom, // left, right
                                      -0.5f * context.zoom, 0.5f * context.zoom, // bottom, top
                                      context.near_plane, context.far_plane);
      },
      std::make_shared<renderer::Simple2DIoControl>(),
      std::make_shared<renderer::PanCamera>(std::array<float, 3>{-0.126, 51, -20.0}), -1000.0, 1000.0);
  auto mesh = std::make_shared<renderer::entities::Mesh2d>(vertices, indices, colors);
  renderer._renderables.emplace_back(mesh, false);
  mesh->set_projection(renderer::eye4d());
  mesh->set_model(renderer::eye4d());
  mesh->set_view(renderer::eye4d());

  auto us = 1us;
  auto start = std::chrono::high_resolution_clock::now();
  while (!renderer.should_close()) {
    renderer.render();
    auto end = std::chrono::high_resolution_clock::now();
    us = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Frame time: " << us.count() << "us\n";
    start = end;
  }
  renderer::terminate();
  return 0;
}
