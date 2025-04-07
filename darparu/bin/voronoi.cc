#include "darparu/renderer/algebra.h"
#include "darparu/renderer/cameras/pan.h"
#include "darparu/renderer/entities/mesh_2d.h"
#include "darparu/renderer/io_controls/simple_2d.h"
#include "darparu/renderer/projection_context.h"
#include "darparu/renderer/renderer.h"
#include "math.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std::chrono_literals;

using namespace darparu;

std::vector<std::vector<float>> load_voronoi_faces(const std::string &file_path) {
  std::ifstream file(file_path);
  if (!file.is_open()) {
    std::cerr << "Error opening file" << std::endl;
    return {};
  }
  std::vector<std::vector<float>> data;
  std::string line;
  // Skip the header line
  std::getline(file, line);
  while (std::getline(file, line)) {
    std::vector<float> row;
    std::stringstream ss(line);
    std::string cell;
    // Skip the first column (RegionID)
    std::getline(ss, cell, ',');
    while (std::getline(ss, cell, ',')) {
      // Remove quotes and split "x,y" into two floats
      cell.erase(remove(cell.begin(), cell.end(), '"'), cell.end());
      std::stringstream pairStream(cell);
      std::string value;
      while (std::getline(pairStream, value, ',')) {
        row.push_back(std::stof(value));
      }
    }
    data.push_back(row);
  }
  file.close();
  return data;
}

std::vector<std::vector<float>> triangulate_polygon(const std::vector<float> &vertices) {
  std::vector<std::vector<float>> triangles;
  int n = vertices.size() / 2;
  if (n < 3)
    return triangles;
  for (int i = 1; i < n - 1; i++) {
    triangles.push_back({vertices[0], vertices[1], vertices[2 * i], vertices[2 * i + 1], vertices[2 * (i + 1)],
                         vertices[2 * (i + 1) + 1]});
  }
  return triangles;
}

int main() {
  std::vector<std::vector<float>> voronoi = load_voronoi_faces("voronoi_faces.csv");

  std::vector<float> vertices;
  std::vector<unsigned int> indices;
  std::vector<float> colors;
  size_t num_regions = voronoi.size();

  size_t voronoi_index = 0;
  size_t current_indice_index = 0;
  while (voronoi.size() > 0) {
    std::cout << "\rProcessing regions: " << (static_cast<double>(voronoi_index) / num_regions) * 100.0 << "%";

    std::vector<float> row = voronoi.back();
    voronoi.pop_back();
    // Triangular the row and store the vertices in the vertices vector
    std::vector<std::vector<float>> triangles = triangulate_polygon(row);
    // Generate colors for each triangle based on the current region index
    float r = std::fmod(static_cast<double>(voronoi_index) / (static_cast<double>(num_regions) / 3.0), 1.0);
    float g = std::fmod(static_cast<double>(voronoi_index) / (static_cast<double>(num_regions) / 2.0), 1.0);
    float b = static_cast<double>(voronoi_index) / (static_cast<double>(num_regions));
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
        return renderer::orthographic(-0.5, 0.5, -0.5, 0.5, context.near_plane, context.far_plane);
      },
      std::make_shared<renderer::Simple2DIoControl>(-0.001, -10.0),
      std::make_shared<renderer::PanCamera>(std::array<float, 3>{0.0, 0.0, -5.0}));

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
