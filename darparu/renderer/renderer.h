#pragma once
#include "darparu/renderer/camera.h"
#include "darparu/renderer/projection_context.h"
#include "darparu/renderer/renderable.h"
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>

namespace darparu::renderer {

void init();

void terminate();

class Renderer {
private:
  GLFWwindow *_window;
  double _scroll_offset;
  double _mouse_position_in_pixels[2];
  double _last_mouse_position_in_pixels[2];
  double _mouse_position_change_in_pixels[2];

  bool _escape_pressed;

  int _window_width, _window_height;
  int _framebuffer_width, _framebuffer_height;

  std::array<float, 16> _projection;
  std::array<float, 16> _view;

  ProjectionFunction _projection_function;
  ProjectionContext _projection_context;

public:
  Renderer(int window_width, int window_height, ProjectionFunction projection_function);
  Renderer(int window_width, int window_height);
  ~Renderer();

  Camera _camera;
  std::vector<std::tuple<std::shared_ptr<Renderable>, bool>> _renderables;
  std::array<float, 2> _camera_radians;
  std::array<float, 3> _camera_position;
  bool _mouse_click;
  void update_camera();
  void render(bool rotate_camera);
  bool should_close();
  std::array<float, 3> get_cursor_direction();

  void set_projection_function(ProjectionFunction projection_function);

private:
  float _near_plane = 0.01f;
  float _far_plane = 100.0f;

  void on_framebuffer_shape_change();
  void update_projection();
  void update_camera(bool rotate_camera);

  GLFWwindow *create_window(int width, int height);
  static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
  static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
  static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
  static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
  static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
};

} // namespace darparu::renderer
