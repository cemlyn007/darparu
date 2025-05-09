#pragma once
#include <GL/glew.h>
#include <array>
#include <string>

namespace darparu::renderer {

class Shader {
private:
  GLuint _program;

public:
  Shader(std::string vertex_source_code, std::string fragment_source_code);
  ~Shader();

  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;

  Shader(Shader &&other) noexcept = delete;
  Shader &operator=(Shader &&other) noexcept = delete;

  void use();
  void unuse();
  void set_uniform(const std::string &name, int value);
  void set_uniform_vector(const std::string &name, const std::array<float, 2> &vector);
  void set_uniform_vector(const std::string &name, const std::array<float, 3> &vector);
  void set_uniform_matrix(const std::string &name, const std::array<float, 16> &matrix);

private:
  GLuint load_program(std::string vertex_source_code, std::string fragment_source_code);
  GLuint load_vertex_shader(std::string vertex_source_code);
  GLuint load_fragment_shader(std::string fragment_source);
};

std::string read_file(const std::string &file_path);

} // namespace darparu::renderer