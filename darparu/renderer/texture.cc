#include "darparu/renderer/texture.h"

namespace darparu::renderer {

Texture::Texture(GLuint texture) : _texture(texture) {}

Texture::~Texture() {}

void Texture::use() {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _texture);
}

void Texture::unuse() { glBindTexture(GL_TEXTURE_2D, 0); }

} // namespace darparu::renderer