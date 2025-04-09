#pragma once
#include "darparu/renderer/algebra.h"
#include <array>

namespace darparu::renderer {
class Camera {
public:
  Camera() = default;
  virtual ~Camera() = default;
  std::array<float, 3> _position;
  std::array<float, 2> _radians;
  float _zoom = 1.0f;

  virtual std::array<float, 16> update() { return look_at(_position, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}); };
};
} // namespace darparu::renderer
