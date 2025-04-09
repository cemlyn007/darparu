#pragma once
#include <array>

namespace darparu::renderer {
class IoControl {
public:
  IoControl() = default;
  ~IoControl() = default;
  bool update();
  void key_event(int key, int scancode, int action, int mods);
  void mouse_button_event(int button, int action, int mods);
  void cursor_position_event(double xpos, double ypos);
  void scroll_event(double xoffset, double yoffset);

  bool _mouse_click = false;
  bool _escape_pressed = false;
  double _scroll_offset = 0.0;
  double _mouse_position_in_pixels[2] = {0.0, 0.0};
  double _last_mouse_position_in_pixels[2] = {0.0, 0.0};
  double _mouse_position_change_in_pixels[2] = {0.0, 0.0};

  bool virtual control(std::array<float, 3> &camera_position, std::array<float, 2> &camera_radians, float &zoom) {
    return false;
  };

private:
  bool _event = false;
};
} // namespace darparu::renderer