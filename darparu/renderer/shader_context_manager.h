#pragma once
#include "darparu/renderer/shader.h"

namespace darparu::renderer {

class ShaderContextManager {
public:
  ShaderContextManager(Shader &shader);
  ~ShaderContextManager();

private:
  Shader &_shader;
};

} // namespace darparu::renderer