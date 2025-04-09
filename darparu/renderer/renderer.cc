#include "darparu/renderer/renderer.h"
#include "darparu/renderer/algebra.h"
#include "darparu/renderer/gl_error_macro.h"
#include <GL/glew.h>
#include <iostream>

static void glfwErrorCallback(int error, const char *description) {
  std::cerr << "GLFW Error: " << error << " - " << description << std::endl;
  throw std::runtime_error(std::string("GLFW Error: ") + description);
}

namespace darparu::renderer {

void init() {
  glfwSetErrorCallback(glfwErrorCallback);
  if (!glfwInit()) {
    throw std::runtime_error("Could not initialize glfw");
  }
  // else...
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void terminate() {
  glfwSetErrorCallback(nullptr);
  glfwTerminate();
}

Renderer::Renderer(std::string window_name, int window_width, int window_height, std::shared_ptr<IoControl> control,
                   std::shared_ptr<Camera> camera, float near_plane, float far_plane)
    : Renderer(
          window_name, window_width, window_height,
          [](const ProjectionContext &context) {
            return perspective(radians(60), context.aspect_ratio, context.near_plane, context.far_plane);
          },
          control, camera, near_plane, far_plane) {}

Renderer::Renderer(std::string window_name, int window_width, int window_height, ProjectionFunction projection_function,
                   std::shared_ptr<IoControl> control, std::shared_ptr<Camera> camera, float near_plane,
                   float far_plane)
    : _window(create_window(window_name, window_width, window_height)), _projection_function(projection_function),
      _io_control(control), _camera(camera), _camera_texture(window_width, window_height), _renderables(),
      _near_plane(near_plane), _far_plane(far_plane) {

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  on_framebuffer_shape_change();

  for (auto [renderable, _] : _renderables) {
    renderable->set_projection(_projection);
    renderable->set_view(_view);
  }

  _io_control->update();
  _io_control->control(_camera->_position, _camera->_radians, _camera->_zoom);
  update_camera();
  _camera_texture.unbind();
}

Renderer::~Renderer() { glfwDestroyWindow(_window); }

void Renderer::render() {
  glfwMakeContextCurrent(_window);

  _camera_texture.bind();
  GL_CALL(glClearColor(0.1, 0.1, 0.1, 1.0));
  GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  for (auto [renderable, reflect_draw] : _renderables) {
    renderable->set_projection(_projection);
    renderable->set_view(_view);
    if (reflect_draw)
      renderable->draw();
  }
  _camera_texture.unbind();

  GL_CALL(glViewport(0, 0, _framebuffer_width, _framebuffer_height));
  GL_CALL(glClearColor(0.1, 0.1, 0.1, 1.0));
  GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  for (auto [renderable, _] : _renderables) {
    renderable->set_projection(_projection);
    renderable->set_view(_view);
    renderable->draw();
  }

  GL_CALL(glfwSwapBuffers(_window));
  if (_io_control->update() && _io_control->control(_camera->_position, _camera->_radians, _camera->_zoom)) {
    update_projection();
    update_camera();
  }
}

void Renderer::on_framebuffer_shape_change() {
  glfwGetFramebufferSize(_window, &_framebuffer_width, &_framebuffer_height);
  glfwGetWindowSize(_window, &_window_width, &_window_height);
  _camera_texture.resize(_framebuffer_width, _framebuffer_height);
  update_projection();
}

void Renderer::update_projection() {
  _projection_context.aspect_ratio = static_cast<float>(_framebuffer_width) / static_cast<float>(_framebuffer_height);
  _projection_context.near_plane = _near_plane;
  _projection_context.far_plane = _far_plane;
  _projection_context.zoom = _camera->_zoom;
  _projection = _projection_function(_projection_context);
  for (auto &[renderable, _] : _renderables)
    renderable->set_projection(_projection);
}

void Renderer::set_projection_function(ProjectionFunction projection_function) {
  _projection_function = std::move(projection_function);
  update_projection();
}

void Renderer::update_camera() {
  _view = _camera->update();
  for (auto &[renderable, _] : _renderables)
    renderable->set_view(_view);
}

bool Renderer::should_close() { return glfwWindowShouldClose(_window) || _io_control->_escape_pressed; }

// Kudos goes to: https://antongerdelan.net/opengl/raycasting.html
std::array<float, 3> Renderer::get_cursor_direction() {
  std::array<float, 3> nds = {
      static_cast<float>((2.0 * _io_control->_mouse_position_in_pixels[0]) / _window_width - 1.0),
      static_cast<float>(1.0 - (2.0 * _io_control->_mouse_position_in_pixels[1]) / _window_height), 1.0f};
  std::array<float, 4> clip = {nds[0], nds[1], -1.0, 1.0f};
  auto inverse_projection = inverse(_projection);
  auto ray_eye_xyzw = multiply_matrix(inverse_projection, clip);
  const std::array<float, 4> t_ray_eye_xyzw = {ray_eye_xyzw[0], ray_eye_xyzw[1], -1.0, 0.0};
  auto inverse_view = inverse(_view);
  auto ray_wor_xyzw = multiply_matrix(inverse_view, t_ray_eye_xyzw);
  std::array<float, 3> ray_wor = {ray_wor_xyzw[0], ray_wor_xyzw[1], ray_wor_xyzw[2]};
  auto arr = normalize(ray_wor);
  return arr;
};

GLFWwindow *Renderer::create_window(std::string window_name, int width, int height) {
  GLFWwindow *window = glfwCreateWindow(width, height, window_name.c_str(), nullptr, nullptr);
  if (!window) {
    throw std::runtime_error("Could not create window");
  }
  // else...
  glfwMakeContextCurrent(window);
  GLenum error = glewInit();
  if (GLEW_OK != error) {
    glfwDestroyWindow(window);
    throw std::runtime_error(std::string("Error initializing glew: ") +
                             reinterpret_cast<const char *>(glewGetErrorString(error)));
  }
  glfwSwapInterval(0);
  glfwGetFramebufferSize(window, &_framebuffer_width, &_framebuffer_height);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetWindowUserPointer(window, this);
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
  glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  glfwSetScrollCallback(window, scroll_callback);
  return window;
}

void Renderer::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  Renderer *renderer = static_cast<Renderer *>(glfwGetWindowUserPointer(window));
  renderer->_io_control->key_event(key, scancode, action, mods);
}

void Renderer::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
  Renderer *renderer = static_cast<Renderer *>(glfwGetWindowUserPointer(window));
  renderer->_io_control->mouse_button_event(button, action, mods);
}

void Renderer::cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
  Renderer *renderer = static_cast<Renderer *>(glfwGetWindowUserPointer(window));
  renderer->_io_control->cursor_position_event(xpos, ypos);
}

void Renderer::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  Renderer *renderer = static_cast<Renderer *>(glfwGetWindowUserPointer(window));
  renderer->_io_control->scroll_event(xoffset, yoffset);
}

void Renderer::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  Renderer *renderer = static_cast<Renderer *>(glfwGetWindowUserPointer(window));

  glfwMakeContextCurrent(window);
  glViewport(0, 0, width, height);
  renderer->on_framebuffer_shape_change();
}

} // namespace darparu::renderer
