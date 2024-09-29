#pragma once

#include <fstream>
#include <memory>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string_view>

class Drawable {
public:
  virtual void draw() = 0;
};

class Bindable {
public:
  virtual void bind() = 0;
  virtual void unbind() = 0;
};

class glObject {
public:
  virtual const GLuint getHandle() = 0;
  operator GLuint() { return getHandle(); }
};

template <typename... T> class Animatable {
public:
  virtual void update(T... v) = 0;
};
