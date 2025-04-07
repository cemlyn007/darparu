#pragma once
#include "darparu/renderer/algebra.h"
#include "darparu/renderer/camera.h"
#include <array>

namespace darparu::renderer {
class PanCamera : public Camera {
public:
  PanCamera() = default;
  PanCamera(std::array<float, 3> position) { _position = position; };
  ~PanCamera() = default;

  virtual std::array<float, 16> update() {
    return look_at(_position, {_position[0], _position[1], 0.0}, {0.0, 1.0, 0.0});
  };
};
} // namespace darparu::renderer
