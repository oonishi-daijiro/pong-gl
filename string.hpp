#pragma once

#include <cmath>
#include <corecrt.h>
#include <cstddef>
#include <cstdlib>
#include <cwchar>
#include <glm/fwd.hpp>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <freetype/fttypes.h>

#include "buffer.hpp"
#include "freetype/freetype.h"
#include "ft2wrap.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "traits.hpp"
#include "window.hpp"

struct CharacterMetrics {
  CharacterMetrics(FT_ULong charcode, const Ft2Wrap::Face &face)
      : charcode(charcode), width(face->glyph->bitmap.width),
        height(face->glyph->bitmap.rows), bearingX(face->glyph->bitmap_left),
        bearingY(face->glyph->bitmap_top),
        advanceX(face->glyph->advance.x >> 6),
        advanceY(face->glyph->advance.y >> 6) {}

  const FT_ULong charcode;

  const unsigned int width, height;
  const int bearingX, bearingY;
  const int advanceX, advanceY;
};

class Character : Drawable, Animatable<glm::vec2>, Animatable<wchar_t> {
private:
  static inline glm::vec2 uv[4]{{0, 1}, {0, 0}, {1, 0}, {1, 1}};

  static inline std::unique_ptr<VertexArray> array{nullptr};
  static inline std::unique_ptr<ArrayBuffer<glm::vec3>> textureVerticesBuffer{
      nullptr};
  static inline std::unique_ptr<ArrayBuffer<glm::vec2>> textureUVcoordsBuffer{
      nullptr};

  static inline std::map<std::string, Ft2Wrap::Face> faces{};
  static inline std::map<FT_ULong, Texture> textureMap{};
  static inline std::map<std::pair<FT_ULong, FT_F26Dot6>, CharacterMetrics>
      charactersMetrics{};

  static inline std::string vertexShaderPath{
      "../shaders/character/character.vert"};
  static inline std::string fragmentShaderPath{
      "../shaders/character/character.frag"};

  static inline void initVertexArray() {
    array = std::make_unique<VertexArray>();
  }

  static inline void innitVerticesBuffer() {
    if (array) {
      textureVerticesBuffer = std::make_unique<ArrayBuffer<glm::vec3>>(4);
      array->vertexArrayVertexBuffer(0, textureVerticesBuffer->getHandle(), 0,
                                     sizeof(float) * 3);

      array->enableVertexArrayAttrib(0);
      array->vertexArrayAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
      array->vertexArrayAttribBinding(0, 0);
    }
  }
  static inline void initTextureUVBuffer() {
    if (array) {
      textureUVcoordsBuffer = std::make_unique<ArrayBuffer<glm::vec2>>(4);
      *textureUVcoordsBuffer = uv;

      array->vertexArrayVertexBuffer(1, textureUVcoordsBuffer->getHandle(), 0,
                                     sizeof(float) * 2);
      array->enableVertexArrayAttrib(1);
      array->vertexArrayAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);
      array->vertexArrayAttribBinding(1, 1);
    }
  }

public:
  Character(std::string fontPath, FT_ULong character, FT_F26Dot6 size,
            glm::vec3 color, glm::vec2 pos)
      : pos{pos}, texturePtr{nullptr}, metricsPtr(nullptr), size(size),
        charcode(character) {
    if (!array) {
      initVertexArray();
    }

    if (!textureVerticesBuffer) {
      innitVerticesBuffer();
    }

    if (!textureUVcoordsBuffer) {
      initTextureUVBuffer();
    }

    Shader vertexShader{vertexShaderPath, GL_VERTEX_SHADER};
    Shader fragmentShader{fragmentShaderPath, GL_FRAGMENT_SHADER};

    vertexShader.compile();
    fragmentShader.compile();

    shader = ShaderProgram{vertexShader, fragmentShader};

    if (!faces.count(fontPath)) {
      faces.emplace(std::piecewise_construct, std::forward_as_tuple(fontPath),
                    std::forward_as_tuple(fontPath));
    }

    auto &face = faces.at(fontPath);
    facePtr = &face;

    if (!charactersMetrics.count(std::pair(character, size))) {
      auto [windowWidth, windowHeight] = Window::getSize();
      face.setCharSize(size, 0, windowWidth, windowHeight);
      auto index = face.getCharIndex(character);
      auto res = face.loadGlypth(index, FT_LOAD_DEFAULT);

      charactersMetrics.emplace(
          std::piecewise_construct,
          std::forward_as_tuple(std::pair(character, size)),
          std::forward_as_tuple(character, face));
    }

    metricsPtr = &charactersMetrics.at(std::pair(character, size));

    calculateVertices();

    if (!textureMap.count(character)) {
      textureMap.emplace(std::piecewise_construct,
                         std::forward_as_tuple(character),
                         std::forward_as_tuple(GL_TEXTURE_2D));
      if (metricsPtr->width > 0 && metricsPtr->height > 0) {
        auto &face = faces.at(fontPath);
        renderToTexture(character, face, textureMap.at(character));
      }
    }
    texturePtr = &textureMap.at(character);

    shader.use();
    glUniform3f(4, color.r, color.g, color.b);
  }

  auto getLineHeight() { return (*facePtr)->size->metrics.height >> 6; }

  virtual void draw() override {
    *textureVerticesBuffer = textureVertices;

    array->bind();
    texturePtr->bind();

    shader.use();

    glUniform1f(2, Window::getAspect());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    texturePtr->unbind();
    array->unbind();
  }

  virtual void update(glm::vec2 newPos) override {
    pos = newPos;
    calculateVertices();
  }

  virtual void update(wchar_t newCharacter) override {
    auto [windowWidth, windowHeight] = Window::getSize();
    if (!textureMap.count(newCharacter)) {
      facePtr->setCharSize(size, 0, windowWidth, windowHeight);
      auto index = facePtr->getCharIndex(newCharacter);
      auto res = facePtr->loadGlypth(index, FT_LOAD_DEFAULT);
      textureMap.emplace(std::piecewise_construct,
                         std::forward_as_tuple(newCharacter),
                         std::forward_as_tuple(GL_TEXTURE_2D));
      renderToTexture(newCharacter, *facePtr, textureMap.at(newCharacter));
    }

    if (!charactersMetrics.count(std::pair{newCharacter, size})) {
      charactersMetrics.emplace(std::piecewise_construct,
                                std::forward_as_tuple(newCharacter, size),
                                std::forward_as_tuple(newCharacter, *facePtr));
    }

    texturePtr = &textureMap.at(newCharacter);
    metricsPtr = &charactersMetrics.at(std::pair{newCharacter, size});
    calculateVertices();
  }

private:
  void calculateVertices() {
    auto [windowWidth, windowHeight] = Window::getSize();

    textureVertices[0] = {pos.x, pos.y, 0};

    textureVertices[1] = {
        pos.x, pos.y + static_cast<float>(metricsPtr->height) / windowHeight,
        0};

    textureVertices[2] = {
        pos.x + (static_cast<float>(metricsPtr->width) / windowWidth),
        pos.y + (static_cast<float>(metricsPtr->height) / windowHeight), 0};

    textureVertices[3] = {
        pos.x + (static_cast<float>(metricsPtr->width) / windowWidth), pos.y,
        0};
  }

  void renderToTexture(FT_ULong character, Ft2Wrap::Face &face,
                       Texture &texture) {

    auto res = face.renderGlyph(FT_RENDER_MODE_NORMAL);

    auto data = face->glyph->bitmap.buffer;

    texture.pixelStorei(GL_UNPACK_ALIGNMENT, 1);
    texture.textureStorage2D(1, GL_R8, face->glyph->bitmap.width,
                             face->glyph->bitmap.rows);

    texture.textureSubImage2D(0, 0, 0, face->glyph->bitmap.width,
                              face->glyph->bitmap.rows, GL_RED,
                              GL_UNSIGNED_BYTE, data);

    texture.getnerateTextureMipmap();

    texture.textureParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    texture.textureParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    texture.textureParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texture.textureParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }

protected:
  friend class String;
  CharacterMetrics *metricsPtr;
  ShaderProgram shader;
  glm::vec2 pos;
  Texture *texturePtr;
  Ft2Wrap::Face *facePtr;
  glm::vec3 textureVertices[4];
  FT_F26Dot6 size;
  FT_ULong charcode;
};

class String : public Drawable, public Animatable<std::wstring_view> {
public:
  enum direction { horizonal, vertical };
  enum justify_mode { left, center };

  String(std::string fontPath, std::wstring str, FT_F26Dot6 size,
         glm::vec3 color, glm::vec2 pos, justify_mode justity = left,
         direction d = horizonal)
      : str{str}, d{d}, pos{pos}, fontPath{fontPath}, size{size}, color{color},
        jmode(justity) {
    auto [windowWidth, windowHeight] = Window::getSize();
    characters.reserve(str.size());
    for (auto &&c : str) {
      characters.emplace_back(fontPath, c, size, color, glm::vec2{0, 0});
    }

    if (d == horizonal) {
      characters.reserve(str.size());
      for (int i = 0; i < characters.size(); i++) {
        characters[i].update(str[i]);
      }

      calculateCharacterHolizonalLayout();
    }
  }

  virtual void update(std::wstring_view newStr) override {
    if (str != newStr) {
      auto [windowWidth, windowHeight] = Window::getSize();
      if (newStr.size() > characters.size()) {
        characters.reserve(newStr.size());
        for (int i = 0; i < characters.size(); i++) {
          characters[i].update(newStr[i]);
        }

        for (int i = characters.size(); i < newStr.size(); i++) {
          characters.emplace_back(fontPath, newStr[i], size, color,
                                  glm::vec2{0, 0});
        }

        if (d == horizonal) {
          calculateCharacterHolizonalLayout();
        }
      } else if (newStr.size() <= characters.size()) {
        characters.erase(characters.begin() + newStr.size(), characters.end());
        for (int i = 0; i < newStr.size(); i++) {
          characters[i].update(newStr[i]);
        }
        if (d == horizonal) {
          calculateCharacterHolizonalLayout();
        }
      }
    }
    str = newStr;
  }

  virtual void draw() override {
    for (auto &&c : characters) {
      c.draw();
    }
  }

private:
  void calculateCharacterHolizonalLayout() {
    glm::vec2 nextPos{0, 0};
    auto [windowWidth, windowHeight] = Window::getSize();

    if (jmode == left) {
      nextPos = pos;
    } else if (jmode == center) {
      auto [windowWidth, windowHeight] = Window::getSize();
      float stringWidth = 0;

      float stringHeight =
          static_cast<float>(
              Character::faces.at(fontPath)->size->metrics.height >> 6) /
          windowHeight;

      for (auto &&c : characters) {
        stringWidth += static_cast<float>(c.metricsPtr->advanceX) / windowWidth;
      }

      nextPos =
          glm::vec2{pos.x - (stringWidth / 2), pos.y - (stringHeight / 4)};
    }

    for (auto &&c : characters) {

      if (c.charcode == L'\n') {
        auto lineHeight =
            Character::faces.at(fontPath)->size->metrics.height >> 6;
        nextPos =
            glm::vec2{pos.x, nextPos.y - ((float)lineHeight / windowHeight)};
        continue;
      }

      c.update(glm::vec2{
          nextPos.x + ((float)c.metricsPtr->bearingX / windowWidth),
          nextPos.y -
              (((float)c.metricsPtr->height - (float)c.metricsPtr->bearingY) /
               windowHeight)});

      nextPos = glm::vec2{
          nextPos.x + (float)c.metricsPtr->advanceX / windowWidth, nextPos.y};
    }
  }

  std::wstring str;
  std::vector<Character> characters;
  glm::vec2 pos;
  std::string fontPath;
  FT_F26Dot6 size;
  glm::vec3 color;
  direction d;
  justify_mode jmode;
};
