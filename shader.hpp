#pragma once

#include <cwchar>
#include <iostream>
#include <map>
#include <memory>
#include <ratio>
#include <string>
#include <string_view>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <tuple>
#include <utility>

#include "handle.hpp"
#include "traits.hpp"

class ShaderProgram : public glObject {
private:
  ShaderProgramHandle handle{};

public:
  ShaderProgram() {}
  template <typename... T> ShaderProgram(T &&...shaders) {

    for (glObject *e : {&shaders...}) {
      glAttachShader(handle, *e);
    }
    glLinkProgram(handle);
  };

  virtual const GLuint getHandle() { return handle; }

  void use() { glUseProgram(handle); };
};

class Shader : public glObject {
  static inline std::map<std::string,
                         std::pair<std::shared_ptr<ShaderHandle>, bool>>
      shaders{};

  static inline std::tuple<std::unique_ptr<char[]>, int>
  readfile(std::string_view filepath) {
    std::unique_ptr<char[]> filedata{};

    std::ifstream file(filepath.data());
    file.seekg(0, std::ios::end);
    size_t length = file.tellg();
    filedata = std::move(std::make_unique<char[]>(length));

    file.seekg(0, std::ios::beg);
    file.read(filedata.get(), length);
    filedata[length - 1] = '\0';
    file.close();
    return {std::move(filedata), length};
  };

public:
  Shader(auto &&src, GLint type)
      : handle{nullptr}, src{std::forward<decltype(src)>(src)} {
    if (!shaders.count(src)) {
      shaders[src].first = std::move(std::make_shared<ShaderHandle>(type));
      shaders[src].second = false;
    }
    handle = shaders[src].first;
  }

  bool compile() {
    if (!shaders[src].second) {
      auto &&[file, size] = readfile(src);
      auto p = file.get();
      auto pp = &p;
      glShaderSource(*handle, 1, pp, &size);
      glCompileShader(*handle);
      shaders[src].second = true;
    }

    GLint status;
    glGetShaderiv(*handle, GL_COMPILE_STATUS, &status);
    compileSatus = status;
    return status;
  }

  bool getCompileStaus() { return compileSatus; };

  std::string getCompileMessage() {
    GLsizei length;
    glGetShaderiv(getHandle(), GL_INFO_LOG_LENGTH, &length);
    std::string msg(length, '\0');
    GLsizei errorLength;
    glGetShaderInfoLog(getHandle(), length, &errorLength, msg.data());
    return msg;
  }

  virtual const GLuint getHandle() override { return *handle; };

private:
  std::shared_ptr<ShaderHandle> handle;
  std::string src;
  bool compileSatus = false;
};
