#include "darparu/renderer/algebra.h"
#include "darparu/renderer/entities/ball.h"
#include "darparu/renderer/entities/container.h"
#include "darparu/renderer/entities/water.h"
#include "darparu/renderer/renderer.h"
#include "math.h"
#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

using namespace darparu;

constexpr size_t RESOLUTION = 101;
constexpr float SPACING = 0.02;
constexpr float WALL_THICKNESS = 0.1;

int main(int argc, char *argv[]) {
  renderer::init();

  renderer::Renderer renderer(1080, 1080, RESOLUTION, SPACING, WALL_THICKNESS);

  std::vector<renderer::BallConfig> ball_configs = {
      {{1.0, 0.0, 0.0}, 0.2}, {{0.0, 1.0, 0.0}, 0.3}, {{0.0, 0.0, 1.0}, 0.25}};

  std::vector<std::array<float, 3>> ball_positions = {{-0.5, 1.0, -0.5}, {0.5, 1.0, -0.5}, {0.5, 1.0, 0.5}};

  auto container = std::make_shared<renderer::entities::Container>((RESOLUTION - 1) * SPACING, WALL_THICKNESS);
  renderer._renderables.emplace_back(container, true);
  container->set_projection(renderer::eye4d());
  float wall_size = (RESOLUTION - 1) * SPACING;
  auto container_water_model = renderer::translate(renderer::eye4d(), {-wall_size / 2.0f, 0.0, -wall_size / 2.0f});
  container->set_color({0.7, 0.7, 0.7});
  container->set_model(renderer::transpose(container_water_model));
  container->set_light_color({1.0, 1.0, 1.0});
  container->set_light_position({1.2, 4.0, 2.0});
  container->set_view_position(renderer._camera_position);
  container->set_view(renderer::eye4d());

  for (size_t sphere = 0; sphere < ball_configs.size(); ++sphere) {
    auto ball = std::make_shared<renderer::entities::Ball>();
    renderer._renderables.emplace_back(ball, true);
    ball->set_projection(renderer::eye4d());
    ball->set_model(renderer::eye4d());
    ball->set_color(ball_configs[sphere].color);

    ball->set_light_color({1.0, 1.0, 1.0});
    ball->set_light_position({1.2, 4.0, 2.0});

    ball->set_view_position(renderer._camera_position);
    ball->set_view(renderer::eye4d());
    const auto radius = ball_configs[sphere].radius;
    ball->set_model(renderer::transpose(
        renderer::translate(renderer::scale(renderer::eye4d(), {radius, radius, radius}), ball_positions[sphere])));
  }

  auto water = std::make_shared<renderer::entities::Water>(RESOLUTION, RESOLUTION * SPACING, 0.0f);
  renderer._renderables.emplace_back(water, false);
  water->set_color({0.0, 0.0, 1.0});
  water->set_model(renderer::transpose(container_water_model));

  water->set_light_color({1.0, 1.0, 1.0});
  water->set_light_position({1.2, 4.0, 2.0});

  auto texture = renderer._camera.texture();
  water->set_texture(texture);

  water->set_heights(std::vector<float>(RESOLUTION * RESOLUTION, 0.8f));

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