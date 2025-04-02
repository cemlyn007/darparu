#include "darparu/renderer/shader_context_manager.h"

namespace darparu::renderer {

ShaderContextManager::ShaderContextManager(Shader &shader) : _shader(shader) { _shader.use(); }

ShaderContextManager::~ShaderContextManager() { _shader.unuse(); }

} // namespace darparu::renderer
