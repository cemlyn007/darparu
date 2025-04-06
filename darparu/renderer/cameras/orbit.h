#pragma once
#include "darparu/renderer/algebra.h"
#include "darparu/renderer/camera.h"
#include <array>
#include <cmath>

namespace darparu::renderer {
class OrbitCamera : public Camera {
public:
  OrbitCamera() = default;
  ~OrbitCamera() = default;

  virtual std::array<float, 16> update() {
    _radians[0] = std::fmod(_radians[0], (2 * M_PI));
    _radians[1] = std::fmod(_radians[1], (2 * M_PI));
    float camera_radius = std::min(std::max(0.0f, norm(_position)), 25.0f);
    _position = update_orbit_camera_position(_radians[0], _radians[1], camera_radius);
    return look_at(_position, {0.0, 0.5, 0.0}, {0.0, 1.0, 0.0});
  };
};
} // namespace darparu::renderer
