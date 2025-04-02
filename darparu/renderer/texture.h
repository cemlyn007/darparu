#pragma once
#include <GL/glew.h>

namespace darparu::renderer {

class Texture {
private:
  GLuint _texture;

public:
  Texture(GLuint _texture);
  ~Texture();

  void use();
  void unuse();
};

} // namespace darparu::renderer