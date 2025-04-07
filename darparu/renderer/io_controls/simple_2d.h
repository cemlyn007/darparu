#pragma once
#include "darparu/renderer/io_control.h"
#include <cmath>

namespace darparu::renderer {

class Simple2DIoControl : public IoControl {
public:
  Simple2DIoControl(float near_plane, float far_plane) : _near_plane(near_plane), _far_plane(far_plane) {}
  virtual ~Simple2DIoControl() = default;

  bool control(std::array<float, 3> &camera_position, std::array<float, 2> &camera_radians) override {
    bool camera_changed = false;
    if (_escape_pressed) {
      return camera_changed;
    }
    if (_mouse_click) {
      camera_changed = true;
      camera_position[0] += _mouse_position_change_in_pixels[0];
      camera_position[1] += _mouse_position_change_in_pixels[1];
    }
    if (_scroll_offset != 0.0) {
      camera_changed = true;
      camera_position[2] =
          std::fmax(_far_plane, std::min(_near_plane, static_cast<float>(camera_position[2] - _scroll_offset)));
    }
    return camera_changed;
  }

private:
  float _near_plane = -0.1f;
  float _far_plane = -100.0f;
};
} // namespace darparu::renderer