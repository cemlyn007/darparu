#pragma once

#include <GL/glew.h>

#include "darparu/renderer/texture.h"

namespace darparu::renderer {
class CameraTexture {
public:
  GLuint rendered_texture;

  CameraTexture(int width, int height);
  ~CameraTexture();

  void resize(int width, int height);
  void bind();
  void unbind();

  Texture texture();

private:
  int _width;
  int _height;
  GLuint _framebuffer;
  GLuint _depth_render_buffer;
};
} // namespace darparu::renderer
