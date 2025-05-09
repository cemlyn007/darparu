#pragma once
#include "darparu/renderer/renderable.h"
#include "darparu/renderer/shader.h"
#include <GL/glew.h>
#include <array>
#include <vector>

namespace darparu::renderer::entities {

class Plane : public Renderable {
public:
  Plane();
  ~Plane();

  void set_view(const std::array<float, 16> &view);
  void set_view_position(const std::array<float, 3> &position);
  void set_projection(const std::array<float, 16> &projection);
  void set_normal_matrix(const std::array<float, 16> &normal_matrix);
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

  GLuint init_vbo(const std::vector<float> &vertices);
  GLuint init_ebo(const std::vector<unsigned int> &indices);
  GLuint init_vao(GLuint vbo, GLuint ebo, const std::vector<float> &vertices);
};

} // namespace darparu::renderer::entities
