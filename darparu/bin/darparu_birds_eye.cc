#include "darparu/renderer/algebra.h"
#include "darparu/renderer/cameras/pan.h"
#include "darparu/renderer/entities/light.h"
#include "darparu/renderer/entities/plane.h"
#include "darparu/renderer/entities/water.h"
#include "darparu/renderer/io_controls/simple_2d.h"
#include "darparu/renderer/projection_context.h"
#include "darparu/renderer/renderer.h"
#include "math.h"
#include <chrono>
#include <functional>
#include <iostream>
using namespace std::chrono_literals;

using namespace darparu;

constexpr size_t RESOLUTION = 101;
constexpr float SPACING = 0.001;

int main(int argc, char *argv[]) {
  renderer::init();

  renderer::Renderer renderer(
      "Darparu", 1080, 1080,
      [](const renderer::ProjectionContext &context) {
        return renderer::orthographic(-5.0, 5.0, -5.0, 5.0, context.near_plane, context.far_plane);
      },
      std::make_shared<renderer::Simple2DIoControl>(), std::make_shared<renderer::PanCamera>());
  auto light = std::make_shared<renderer::entities::Light>();
  renderer._renderables.emplace_back(light, false);
  auto light_position = std::array<float, 3>{0.0, 0.0, -0.5};
  light->set_projection(renderer::eye4d());
  light->set_model(
      renderer::transpose(renderer::translate(renderer::scale(renderer::eye4d(), {0.2, 0.2, 0.2}), light_position)));
  light->set_color({1.0, 1.0, 1.0});
  light->set_view(renderer::eye4d());
  light->set_color({1.0, 1.0, 1.0});

  std::vector<std::function<void(const std::array<float, 3> &)>> lambda;

  auto plane = std::make_shared<renderer::entities::Plane>();
  renderer._renderables.emplace_back(plane, false);
  plane->set_projection(renderer::eye4d());
  // Note to take care about set normal matrix,
  //  since it should only be fed rotations.
  auto plane_model_rotation = renderer::transpose(renderer::rotate(renderer::eye4d(), {1.0, 0.0, 0.0}, M_PI_2));
  plane->set_normal_matrix(plane_model_rotation);
  plane->set_color({0.7, 0.7, 0.7});
  plane->set_model(plane_model_rotation);
  plane->set_light_color({1.0, 1.0, 1.0});
  plane->set_light_position(light_position);
  plane->set_view_position(renderer._camera->_position);
  plane->set_view(renderer::eye4d());
  lambda.emplace_back([plane](const std::array<float, 3> &view_position) { plane->set_view_position(view_position); });

  auto water = std::make_shared<renderer::entities::Water>(RESOLUTION, 0.0f);
  renderer._renderables.emplace_back(water, false);
  water->set_color({0.0, 0.0, 1.0});
  water->set_model(renderer::transpose(renderer::rotate(
      renderer::scale(renderer::eye4d(), {RESOLUTION * SPACING, 1.0, RESOLUTION * SPACING}), {1.0, 0.0, 0.0}, M_PI_2)));

  water->set_light_color({1.0, 1.0, 1.0});
  water->set_light_position(light_position);

  auto texture = renderer._camera_texture.texture();
  water->set_texture(texture);

  water->set_heights(std::vector<float>(RESOLUTION * RESOLUTION, 0.1f));
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