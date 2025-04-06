#pragma once
#include <array>

namespace darparu::renderer {
class Camera {
public:
  Camera() = default;
  ~Camera() = default;
  std::array<float, 3> _position;
  std::array<float, 2> _radians;
};
} // namespace darparu::renderer
