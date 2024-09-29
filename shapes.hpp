#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "polygon.hpp"

namespace Shapes {
class Rectangle : public Animatable<glm::vec2, float, float>, public Drawable {
public:
  Rectangle(auto &&pos, float width, float height, auto &&color) {
    std::vector<glm::vec3> tmpcolors(4);
    for (auto &e : tmpcolors) {
      e = color;
    }
    colors = tmpcolors;
    update({pos.x, pos.y}, width, height);
  }

  virtual void update(glm::vec2 pos, float width, float height) override {
    this->x = pos.x;
    this->y = pos.y;
    this->width = width;
    this->height = height;

    glm::vec3 vertices[]{
        {x - (width / 2), y + (height / 2), 0},
        {x - (width / 2), y - (height / 2), 0},
        {x + (width / 2), y + (height / 2), 0},
        {x + (width / 2), y - (height / 2), 0},
    };

    ply.update(vertices, colors);
  }

  virtual void draw() override { ply.draw(); }

  std::tuple<float, float> getPos() { return {x, y}; }
  std::tuple<float, float> getSize() { return {width, height}; }

private:
  Shapes::Polygon ply{4, 4};
  std::vector<glm::vec3> colors;
  float x, y, width, height;
};
} // namespace Shapes
