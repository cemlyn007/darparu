#include "darparu/renderer/algebra.h"
#include "darparu/renderer/cameras/pan.h"
#include "darparu/renderer/entities/mesh_2d.h"
#include "darparu/renderer/io_controls/simple_2d.h"
#include "darparu/renderer/projection_context.h"
#include "darparu/renderer/renderer.h"
#include "math.h"
#include <chrono>
#include <cstdlib>
#include <vector>

using namespace std::chrono_literals;

using namespace darparu;

int main() {
  std::vector<float> vertices = {0.0, 0.0, 0.0, 1.0, 1.0, 1.0,
                                 //
                                 1.0, 1.0, 1.0, 0.0, 0.0, 0.0};
  std::vector<unsigned int> indices = {2, 1, 0, 5, 4, 3};
  std::vector<float> colors = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0,
                               1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};

  renderer::init();

  renderer::Renderer renderer(
      "Darparu", 1080, 1080,
      [](const renderer::ProjectionContext &context) {
        return renderer::orthographic(-0.5f * context.zoom, 0.5f * context.zoom, // left, right
                                      -0.5f * context.zoom, 0.5f * context.zoom, // bottom, top
                                      context.near_plane, context.far_plane);
      },
      std::make_shared<renderer::Simple2DIoControl>(),
      std::make_shared<renderer::PanCamera>(std::array<float, 3>{0.5f, 0.5f, -50.0f}), -1000.0, 1000.0);

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
