#include "darparu/renderer/algebra.h"
#include "darparu/renderer/cameras/pan.h"
#include "darparu/renderer/entities/mesh_2d.h"
#include "darparu/renderer/io_controls/simple_2d.h"
#include "darparu/renderer/projection_context.h"
#include "darparu/renderer/renderer.h"
#include "darparu/triangulate_2d.h"
#include "math.h"
#include <algorithm>
#include <chrono>
#include <cstdlib>
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
      std::make_shared<renderer::Simple2DIoControl>(0.01, 0.01),
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
