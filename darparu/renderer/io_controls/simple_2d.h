#pragma once
#include "darparu/renderer/io_control.h"

namespace darparu::renderer {

class Simple2DIoControl : public IoControl {
public:
  // Public attributes for sensitivities
  float _mouse_sensitivity;
  float _scroll_sensitivity;

  // Constructor with parameters and default values
  Simple2DIoControl(float mouse_sensitivity = 1.0f, float scroll_sensitivity = 1.0f)
      : _mouse_sensitivity(mouse_sensitivity), _scroll_sensitivity(scroll_sensitivity) {}

  virtual ~Simple2DIoControl() = default;

  bool control(std::array<float, 3> &camera_position, std::array<float, 2> &camera_radians, float &zoom) override {
    bool camera_changed = false;
    if (_escape_pressed) {
      return camera_changed;
    }
    if (_mouse_click) {
      camera_changed = true;
      camera_position[0] += _mouse_position_change_in_pixels[0] * _mouse_sensitivity;
      camera_position[1] += _mouse_position_change_in_pixels[1] * _mouse_sensitivity;
    }
    if (_scroll_offset != 0.0) {
      camera_changed = true;
      zoom *= (1.0f - static_cast<float>(_scroll_offset) * _scroll_sensitivity);
    }
    return camera_changed;
  }
};
} // namespace darparu::renderer