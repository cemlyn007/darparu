#include "darparu/renderer/algebra.h"
#include "darparu/renderer/cameras/orbit.h"
#include "darparu/renderer/entities/ball.h"
#include "darparu/renderer/entities/container.h"
#include "darparu/renderer/entities/light.h"
#include "darparu/renderer/entities/water.h"
#include "darparu/renderer/io_controls/simple_3d.h"
#include "darparu/renderer/renderer.h"
#include "math.h"
#include <chrono>
#include <functional>
#include <iostream>

using namespace std::chrono_literals;

using namespace darparu;

constexpr size_t RESOLUTION = 101;
constexpr float SPACING = 0.02;
constexpr float WALL_THICKNESS = 0.1;

struct BallConfig {
  std::array<float, 3> color;
  std::array<float, 3> position;
  float radius;
};

int main(int argc, char *argv[]) {
  renderer::init();

  renderer::Renderer renderer("Darparu", 1080, 1080, std::make_shared<renderer::Simple3DIoControl>(),
                              std::make_shared<renderer::OrbitCamera>());

  std::vector<BallConfig> ball_configs = {{{1.0, 0.0, 0.0}, {-0.5, 1.0, -0.5}, 0.2},
                                          {{0.0, 1.0, 0.0}, {0.5, 1.0, -0.5}, 0.3},
                                          {{0.0, 0.0, 1.0}, {0.5, 1.0, 0.5}, 0.25}};

  auto light = std::make_shared<renderer::entities::Light>();
  renderer._renderables.emplace_back(light, true);
  auto light_position = std::array<float, 3>{0.0, 4.0, 0.0};
  light->set_projection(renderer::eye4d());
  light->set_model(
      renderer::transpose(renderer::translate(renderer::scale(renderer::eye4d(), {0.2, 0.2, 0.2}), light_position)));
  light->set_color({1.0, 1.0, 1.0});
  light->set_view(renderer::eye4d());
  light->set_color({1.0, 1.0, 1.0});

  std::vector<std::function<void(const std::array<float, 3> &)>> lambda;

  auto container = std::make_shared<renderer::entities::Container>((RESOLUTION - 1) * SPACING, WALL_THICKNESS);
  renderer._renderables.emplace_back(container, true);
  container->set_projection(renderer::eye4d());
  auto container_water_model = renderer::eye4d();
  container->set_color({0.7, 0.7, 0.7});
  container->set_model(renderer::transpose(container_water_model));
  container->set_light_color({1.0, 1.0, 1.0});
  container->set_light_position(light_position);
  container->set_view_position(renderer._camera->_position);
  container->set_view(renderer::eye4d());
  lambda.emplace_back(
      [container](const std::array<float, 3> &view_position) { container->set_view_position(view_position); });

  for (size_t sphere = 0; sphere < ball_configs.size(); ++sphere) {
    auto ball = std::make_shared<renderer::entities::Ball>();
    renderer._renderables.emplace_back(ball, true);
    ball->set_projection(renderer::eye4d());
    ball->set_model(renderer::eye4d());
    ball->set_color(ball_configs[sphere].color);

    ball->set_light_color({1.0, 1.0, 1.0});
    ball->set_light_position(light_position);

    ball->set_view_position(renderer._camera->_position);
    ball->set_view(renderer::eye4d());
    const auto radius = ball_configs[sphere].radius;
    ball->set_model(renderer::transpose(renderer::translate(
        renderer::scale(renderer::eye4d(), {radius, radius, radius}), ball_configs[sphere].position)));
    lambda.emplace_back([ball](const std::array<float, 3> &view_position) { ball->set_view_position(view_position); });
  }

  auto water = std::make_shared<renderer::entities::Water>(RESOLUTION, RESOLUTION * SPACING, 0.0f);
  renderer._renderables.emplace_back(water, false);
  water->set_color({0.0, 0.0, 1.0});
  water->set_model(renderer::transpose(container_water_model));

  water->set_light_color({1.0, 1.0, 1.0});
  water->set_light_position(light_position);

  auto texture = renderer._camera_texture.texture();
  water->set_texture(texture);

  water->set_heights(std::vector<float>(RESOLUTION * RESOLUTION, 0.8f));
  lambda.emplace_back([water](const std::array<float, 3> &view_position) { water->set_view_position(view_position); });

  auto us = 1us;
  auto start = std::chrono::high_resolution_clock::now();
  while (!renderer.should_close()) {
    for (const auto &lambda : lambda) {
      lambda(renderer._camera->_position);
    }
    renderer.render();
    auto end = std::chrono::high_resolution_clock::now();
    us = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Frame time: " << us.count() << "us\n";
    start = end;
  }
  renderer::terminate();
  return 0;
}