#include "darparu/renderer/entities/mesh_2d.h"
#include "darparu/renderer/shader.h"
#include "darparu/renderer/shader_context_manager.h"

#include <GL/glew.h>

#include <array>
#include <string>
#include <vector>

namespace darparu::renderer::entities {

std::vector<float> interleave_vertices_and_colors(const std::vector<float> &vertices,
                                                  const std::vector<float> &colors) {
  if (vertices.size() % 2 != 0 || colors.size() % 3 != 0) {
    throw std::invalid_argument("Invalid vertices or colors size: vertices size = " + std::to_string(vertices.size()) +
                                ", colors size = " + std::to_string(colors.size()));
  }
  if (vertices.size() / 2 != colors.size() / 3) {
    throw std::invalid_argument(
        "Vertices and colors size mismatch: vertices count = " + std::to_string(vertices.size() / 2) +
        ", colors count = " + std::to_string(colors.size() / 3));
  }
  std::vector<float> interleaved;
  for (size_t i = 0; i < vertices.size() / 2; ++i) {
    interleaved.push_back(vertices[i * 2]);
    interleaved.push_back(vertices[i * 2 + 1]);
    interleaved.push_back(colors[i * 3]);
    interleaved.push_back(colors[i * 3 + 1]);
    interleaved.push_back(colors[i * 3 + 2]);
  }
  return interleaved;
}

Mesh2d::Mesh2d(std::vector<float> vertices, std::vector<unsigned int> indices, std::vector<float> colors)
    : _shader(read_file("darparu/renderer/shaders/simple_2d.vs"), read_file("darparu/renderer/shaders/simple_2d.fs")),
      _vbo(0), _vao(0), _ebo(0), _num_indices(indices.size()) {

  auto vertices_and_colors = interleave_vertices_and_colors(vertices, colors);

  _vbo = init_vbo(vertices_and_colors);
  _ebo = init_ebo(indices);
  _vao = init_vao(_vbo, _ebo, vertices_and_colors);

  glBindVertexArray(0);
}

Mesh2d::~Mesh2d() {
  glBindVertexArray(0);
  if (_vbo != 0)
    glDeleteBuffers(1, &_vbo);
  if (_ebo != 0)
    glDeleteBuffers(1, &_ebo);
  if (_vao != 0)
    glDeleteVertexArrays(1, &_vao);
}

GLuint Mesh2d::init_vbo(const std::vector<float> &vertices) {
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
  return vbo;
}

GLuint Mesh2d::init_ebo(const std::vector<unsigned int> &indices) {
  GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
  return ebo;
}

GLuint Mesh2d::init_vao(GLuint vbo, GLuint ebo, const std::vector<float> &vertices) {
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // Vertices are 2D (x, y) and colors are 3D (r, g, b)
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  return vao;
}

void Mesh2d::set_view(const std::array<float, 16> &view) {
  ShaderContextManager context(_shader);
  {
    _shader.set_uniform_matrix("view", view);
  }
}

void Mesh2d::set_projection(const std::array<float, 16> &projection) {
  ShaderContextManager context(_shader);
  {
    _shader.set_uniform_matrix("projection", projection);
  }
}

void Mesh2d::set_model(const std::array<float, 16> &model) {
  ShaderContextManager context(_shader);
  {
    _shader.set_uniform_matrix("model", model);
  }
}

void Mesh2d::draw() {
  ShaderContextManager context(_shader);
  {
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, _num_indices, GL_UNSIGNED_INT, nullptr);
  }
}
} // namespace darparu::renderer::entities