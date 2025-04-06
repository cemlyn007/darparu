#pragma once
#include "darparu/renderer/algebra.h"
#include "darparu/renderer/io_control.h"

namespace darparu::renderer {

class Simple3DIoControl : public IoControl {
public:
  Simple3DIoControl() = default;
  virtual ~Simple3DIoControl() = default;

  bool control(std::array<float, 3> &camera_position, std::array<float, 2> &camera_radians) override {
    bool camera_changed = false;
    if (_escape_pressed) {
      return camera_changed;
    }
    if (_mouse_click) {
      camera_changed = true;
      camera_radians[0] += radians(_mouse_position_change_in_pixels[0]);
      camera_radians[1] += radians(_mouse_position_change_in_pixels[1]);
    }
    if (_scroll_offset != 0.0) {
      camera_changed = true;
      float camera_radius = norm(camera_position) + _scroll_offset;
      auto new_camera_position = multiply(normalize(camera_position), camera_radius);
      camera_position[0] = new_camera_position[0];
      camera_position[1] = new_camera_position[1];
      camera_position[2] = new_camera_position[2];
    }
    return camera_changed;
  }
};
} // namespace darparu::renderer