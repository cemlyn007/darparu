#include "darparu/renderer/io_control.h"
#include "darparu/renderer/gl_error_macro.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace darparu::renderer {

bool IoControl::update() {
  _event = false;
  _last_mouse_position_in_pixels[0] = _mouse_position_in_pixels[0];
  _last_mouse_position_in_pixels[1] = _mouse_position_in_pixels[1];
  _scroll_offset = 0.0;
  GL_CALL(glfwPollEvents());
  _mouse_position_change_in_pixels[0] = _mouse_position_in_pixels[0] - _last_mouse_position_in_pixels[0];
  _mouse_position_change_in_pixels[1] = _mouse_position_in_pixels[1] - _last_mouse_position_in_pixels[1];
  return _event;
}

void IoControl::key_event(int key, int scancode, int action, int mods) {
  _event = true;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    _escape_pressed = true;
  }
}

void IoControl::mouse_button_event(int button, int action, int mods) {
  _event = true;
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    _mouse_click = (action == GLFW_PRESS);
  }
}

void IoControl::cursor_position_event(double xpos, double ypos) {
  _event = true;
  _mouse_position_in_pixels[0] = xpos;
  _mouse_position_in_pixels[1] = ypos;
}

void IoControl::scroll_event(double xoffset, double yoffset) {
  _event = true;
  _scroll_offset = yoffset;
}

} // namespace darparu::renderer