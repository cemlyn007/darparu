#pragma once
#include "darparu/renderer/renderable.h"
#include "darparu/renderer/shader.h"
#include <GL/glew.h>
#include <array>
#include <vector>

namespace darparu::renderer::entities {

class Ball : public Renderable {
public:
  Ball();
  ~Ball();

  Ball(const Ball &) = delete;
  Ball &operator=(const Ball &) = delete;

  Ball(Ball &&other) = delete;
  Ball &operator=(Ball &&other) = delete;

  void set_view(const std::array<float, 16> &view);
  void set_view_position(const std::array<float, 3> &position);
  void set_projection(const std::array<float, 16> &projection);
  void set_model(const std::array<float, 16> &model);
  void set_color(const std::array<float, 3> &color);
  void set_light_position(const std::array<float, 3> &position);
  void set_light_color(const std::array<float, 3> &color);

  void draw();

private:
  Shader _shader;
  GLuint _vbo;
  GLuint _vao;
  GLuint _ebo;
  size_t _indices;

  GLuint init_vbo(const std::vector<float> &vertices);
  GLuint init_ebo(const std::vector<unsigned int> &indices);
  GLuint init_vao(GLuint vbo, GLuint ebo);
};

} // namespace darparu::renderer::entities
