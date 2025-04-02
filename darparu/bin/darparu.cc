#include "darparu/renderer/algebra.h"
#include "darparu/renderer/renderer.h"
#include "math.h"
#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

using namespace darparu;

constexpr size_t RESOLUTION = 101;
constexpr float SPACING = 0.02;
constexpr float WALL_THICKNESS = 0.1;
constexpr float WALL_HEIGHT = 1.25;

int main(int argc, char *argv[]) {
  renderer::init();

  renderer::Renderer renderer(1080, 1080, RESOLUTION, SPACING, WALL_THICKNESS,
                              {{{1.0, 0.0, 0.0}, 0.2}, {{0.0, 1.0, 0.0}, 0.3}, {{0.0, 0.0, 1.0}, 0.25}});

  auto us = 1us;
  auto start = std::chrono::high_resolution_clock::now();
  std::optional<std::pair<size_t, float>> selected_sphere;
  std::array<float, 3> last_position;
  bool rotate_camera = false;
  bool just_selected = false;
  while (!renderer.should_close()) {
    renderer.render(rotate_camera);
    auto end = std::chrono::high_resolution_clock::now();
    us = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Frame time: " << us.count() << "us\n";
    start = end;
  }
  renderer::terminate();
  return 0;
}