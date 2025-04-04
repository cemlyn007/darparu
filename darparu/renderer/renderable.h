#pragma once
#include <array>
namespace darparu::renderer {

class Renderable {
public:
  virtual ~Renderable() = default;

  // Method to render the object
  virtual void draw() = 0;

  // Methods to set transformation matrices
  virtual void set_view(const std::array<float, 16> &view) = 0;
  virtual void set_projection(const std::array<float, 16> &projection) = 0;
  virtual void set_model(const std::array<float, 16> &model) = 0;

  // Methods to set object and light properties
  virtual void set_color(const std::array<float, 3> &color) = 0;
  virtual void set_light_position(const std::array<float, 3> &position) = 0;
  virtual void set_light_color(const std::array<float, 3> &color) = 0;

  // Method to set the view position
  virtual void set_view_position(const std::array<float, 3> &position) = 0;
};

} // namespace darparu::renderer