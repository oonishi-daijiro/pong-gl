
#pragma once

#include <cmath>
#include <corecrt.h>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <opencv2/opencv.hpp>

#include "buffer.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "traits.hpp"
#include "window.hpp"

class Image : public Drawable {
public:
  Image(std::string_view filename)
      : array{}, textureVerticesBuf{textureVertices}, uvBuf{uv},
        texture{GL_TEXTURE_2D} {

    auto img = cv::imread(filename.data());
    if (img.empty()) {
      std::cout << "image is empty" << std::endl;
    }

    auto width = img.size().width;
    auto height = img.size().height;
    auto data = img.data;

    Shader vertexShader{vertexShaderPath, GL_VERTEX_SHADER};
    Shader fragmentShader{fragmentShaderPath, GL_FRAGMENT_SHADER};

    vertexShader.compile();
    fragmentShader.compile();

    shader = ShaderProgram{vertexShader, fragmentShader};

    texture.textureStorage2D(1, GL_RGBA8, width, height);
    texture.textureSubImage2D(0, 0, 0, width, height, GL_BGR_EXT,
                              GL_UNSIGNED_BYTE, data);

    texture.pixelStorei(GL_UNPACK_ALIGNMENT, 1);
    texture.getnerateTextureMipmap();

    texture.textureParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    texture.textureParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    texture.textureParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texture.textureParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    array.vertexArrayVertexBuffer(0, textureVerticesBuf, 0, sizeof(float) * 3);
    array.vertexArrayVertexBuffer(1, uvBuf, 0, sizeof(float) * 2);

    array.enableVertexArrayAttrib(0);
    array.enableVertexArrayAttrib(1);

    array.vertexArrayAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    array.vertexArrayAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);

    array.vertexArrayAttribBinding(0, 0);
    array.vertexArrayAttribBinding(1, 1);

    shader.use();
    glUniform1f(2, Window::getAspect());
    glUniform1i(3, 0);
  }

  virtual void draw() override {
    shader.use();
    texture.bind();
    array.bind();
    glUniform1f(2, Window::getAspect());
    glDrawArrays(GL_TRIANGLE_FAN, 0, std::size(textureVertices));
    texture.unbind();
    array.unbind();
  }

private:
  glm::vec3 textureVertices[4]{
      {0.2, 0.2, 0},
      {-0.2, 0.2, 0},
      {-0.2, -0.2, 0},
      {0.2, -0.2, 0},
  };

  glm::vec2 uv[4]{
      {1, 0},
      {0, 0},
      {0, 1},
      {1, 1},
  };

  VertexArray array;
  ArrayBuffer<glm::vec3> textureVerticesBuf;
  ArrayBuffer<glm::vec2> uvBuf;
  Texture texture;

  ShaderProgram shader;
  static inline std::string vertexShaderPath{"../shaders/image/image.vert"};
  static inline std::string fragmentShaderPath{"../shaders/image/image.frag"};
};
