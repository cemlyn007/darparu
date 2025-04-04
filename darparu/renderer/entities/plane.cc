#include "darparu/renderer/entities/plane.h"
#include "darparu/renderer/shader.h"
#include "darparu/renderer/shader_context_manager.h"

#include <GL/glew.h>

#include <array>
#include <string>
#include <vector>

namespace darparu::renderer::entities {

struct MeshData {
  std::vector<float> vertices;
  std::vector<unsigned int> indices;
};

MeshData create_mesh() {
  MeshData mesh;
  mesh.vertices = {0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0};
  mesh.indices = {2, 1, 0, 0, 3, 2};
  return mesh;
}

Plane::Plane()
    : _shader(read_file("darparu/renderer/shaders/simple.vs"), read_file("darparu/renderer/shaders/simple.fs")),
      _vbo(0), _vao(0), _ebo(0) {
  MeshData mesh_data = create_mesh();

  _vbo = init_vbo(mesh_data.vertices);
  _ebo = init_ebo(mesh_data.indices);
  _vao = init_vao(_vbo, _ebo, mesh_data.vertices);

  glBindVertexArray(0);
}

Plane::~Plane() {
  glBindVertexArray(0);
  if (_vbo != 0)
    glDeleteBuffers(1, &_vbo);
  if (_ebo != 0)
    glDeleteBuffers(1, &_ebo);
  if (_vao != 0)
    glDeleteVertexArrays(1, &_vao);
}

GLuint Plane::init_vbo(const std::vector<float> &vertices) {
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
  return vbo;
}

GLuint Plane::init_ebo(const std::vector<unsigned int> &indices) {
  GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
  return ebo;
}

GLuint Plane::init_vao(GLuint vbo, GLuint ebo, const std::vector<float> &vertices) {
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  return vao;
}

void Plane::set_view(const std::array<float, 16> &view) {
  ShaderContextManager context(_shader);
  {
    _shader.set_uniform_matrix("view", view);
  }
}

void Plane::set_view_position(const std::array<float, 3> &position) {
  ShaderContextManager context(_shader);
  {
    _shader.set_uniform_vector("viewPos", position);
  }
}

void Plane::set_projection(const std::array<float, 16> &projection) {
  ShaderContextManager context(_shader);
  {
    _shader.set_uniform_matrix("projection", projection);
  }
}

void Plane::set_model(const std::array<float, 16> &model) {
  ShaderContextManager context(_shader);
  {
    _shader.set_uniform_matrix("model", model);
  }
}

void Plane::set_color(const std::array<float, 3> &color) {
  ShaderContextManager context(_shader);
  {
    _shader.set_uniform_vector("objectColor", color);
  }
}

void Plane::set_light_position(const std::array<float, 3> &position) {
  ShaderContextManager context(_shader);
  {
    _shader.set_uniform_vector("lightPos", position);
  }
}

void Plane::set_light_color(const std::array<float, 3> &color) {
  ShaderContextManager context(_shader);
  {
    _shader.set_uniform_vector("lightColor", color);
  }
}

void Plane::draw() {
  ShaderContextManager context(_shader);
  {
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, 150, GL_UNSIGNED_INT, nullptr);
  }
}
} // namespace darparu::renderer::entities