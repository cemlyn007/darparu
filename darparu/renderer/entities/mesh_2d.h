#pragma once
#include "darparu/renderer/renderable.h"
#include "darparu/renderer/shader.h"
#include <GL/glew.h>
#include <array>
#include <vector>

namespace darparu::renderer::entities {

class Mesh2d : public Renderable {
public:
  Mesh2d(std::vector<float> vertices, std::vector<unsigned int> indices, std::vector<float> colors);
  ~Mesh2d();

  void set_view(const std::array<float, 16> &view);
  void set_projection(const std::array<float, 16> &projection);
  void set_model(const std::array<float, 16> &model);

  void draw();

private:
  Shader _shader;
  GLuint _vbo;
  GLuint _vao;
  GLuint _ebo;
  const size_t _num_indices;

  GLuint init_vbo(const std::vector<float> &vertices);
  GLuint init_ebo(const std::vector<unsigned int> &indices);
  GLuint init_vao(GLuint vbo, GLuint ebo, const std::vector<float> &vertices);
};

} // namespace darparu::renderer::entities
