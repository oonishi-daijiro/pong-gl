#pragma once

#include <cmath>
#include <cstddef>
#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "buffer.hpp"
#include "shader.hpp"
#include "traits.hpp"
#include "window.hpp"

namespace Shapes {
class Polygon : public Animatable<std::span<glm::vec3>, std::span<glm::vec3>>,
                public Drawable {
public:
  static inline const std::string fragmentshaderPath{
      "../shaders/identity/identity.frag"};
  static inline const std::string vertexShaderPath{
      "../shaders/identity/identity.vert"};

  using vertex_t = glm::vec3;
  using color_t = glm::vec3;

  using vertices_t = std::span<vertex_t>;
  using colors_t = std::span<color_t>;

  Polygon(size_t verticesSize, size_t colorSize)
      : vertexArray{verticesSize}, colorArray{colorSize},
        verticesSize(verticesSize) {
    Shader vertexShader{vertexShaderPath, GL_VERTEX_SHADER};
    Shader fragmentShader{fragmentshaderPath, GL_FRAGMENT_SHADER};

    vertexShader.compile();
    fragmentShader.compile();

    shader = ShaderProgram{vertexShader, fragmentShader};
    initShaderInput();
  }

  Polygon(auto &&vertices, auto &&colorVec)
    requires(std::is_array_v<std::remove_reference_t<decltype(vertices)>> &&
             std::is_array_v<std::remove_reference_t<decltype(colorVec)>>)
      : verticesSize{std::size(vertices)}, vertexArray{vertices},
        colorArray{colorVec}, array{} {

    Shader vertexShader{vertexShaderPath, GL_VERTEX_SHADER};
    Shader fragmentShader{fragmentshaderPath, GL_FRAGMENT_SHADER};

    vertexShader.compile();
    fragmentShader.compile();

    initShaderInput();
  }

  virtual void draw() override {
    array.bind();
    shader.use();
    glUniform1f(2, Window::getAspect());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, verticesSize);
    array.unbind();
  }

  virtual void update(vertices_t vertices, colors_t colors) override {
    vertexArray = vertices;
    colorArray = colors;
  }

private:
  void initShaderInput() {
    array.vertexArrayVertexBuffer(0, vertexArray, 0, sizeof(float) * 3);
    array.vertexArrayVertexBuffer(1, colorArray, 0, sizeof(float) * 3);

    array.enableVertexArrayAttrib(0);
    array.enableVertexArrayAttrib(1);

    array.vertexArrayAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    array.vertexArrayAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);

    array.vertexArrayAttribBinding(0, 0);
    array.vertexArrayAttribBinding(1, 1);

    shader.use();
    glUniform1f(2, Window::getAspect());
  }

  size_t verticesSize = 0;

  ShaderProgram shader;

  ArrayBuffer<vertex_t> vertexArray;
  ArrayBuffer<color_t> colorArray;

  VertexArray array;
};
} // namespace Shapes
