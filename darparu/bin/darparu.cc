#include "darparu/renderer/algebra.h"
#include "darparu/renderer/entities/ball.h"
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

  renderer::Renderer renderer(1080, 1080, RESOLUTION, SPACING, WALL_THICKNESS);

  std::vector<renderer::BallConfig> ball_configs = {
      {{1.0, 0.0, 0.0}, 0.2}, {{0.0, 1.0, 0.0}, 0.3}, {{0.0, 0.0, 1.0}, 0.25}};

  std::vector<std::array<float, 3>> ball_positions = {{-0.5, 1.0, -0.5}, {0.5, 1.0, -0.5}, {0.5, 1.0, 0.5}};

  for (size_t sphere = 0; sphere < ball_configs.size(); ++sphere) {
    std::cout << "Creating ball " << sphere << " with radius " << ball_configs[sphere].radius << "\n";
    auto ball = std::make_shared<renderer::entities::Ball>();
    renderer._renderables.emplace_back(ball);
    ball->set_projection(renderer::eye4d());
    ball->set_model(renderer::eye4d());
    ball->set_color(ball_configs[sphere].color);

    ball->set_light_color({1.0, 1.0, 1.0});
    ball->set_light_position({1.2, 4.0, 2.0});

    ball->set_view_position(renderer._camera_position);
    ball->set_view(renderer::eye4d());
    // The multiply by 2 for the radii is because the balls are drawn with a radius of 0.5
    const auto radius = ball_configs[sphere].radius;
    ball->set_model(renderer::transpose(renderer::translate(
        renderer::scale(renderer::eye4d(), {radius * 2, radius * 2, radius * 2}), ball_positions[sphere])));
  }
  renderer._renderables = renderer._renderables;

  auto us = 1us;
  auto start = std::chrono::high_resolution_clock::now();
  while (!renderer.should_close()) {
    renderer.render(renderer._mouse_click);
    auto end = std::chrono::high_resolution_clock::now();
    us = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Frame time: " << us.count() << "us\n";
    start = end;
  }
  renderer::terminate();
  return 0;
}