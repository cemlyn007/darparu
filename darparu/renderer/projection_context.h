#pragma once
#include <array>
#include <functional>

namespace darparu::renderer {

struct ProjectionContext {
  float aspect_ratio;
  float near_plane;
  float far_plane;
  float zoom;
};

using ProjectionFunction = std::function<std::array<float, 16>(const ProjectionContext &)>;

} // namespace darparu::renderer
