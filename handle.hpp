#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <ostream>
#include <utility>

template <typename ALLOCATOR> class Handle {
public:
  template <typename... T> Handle(T &&...args) {
    handle = allocator.alloc(std::forward<T>(args)...);
  }

  Handle(const Handle &) = delete;
  Handle &operator=(const Handle &) = delete;

  Handle(Handle &&handle) { move(std::forward<Handle>(handle)); }

  Handle &operator=(Handle &&handle) {
    move(std::forward<Handle>(handle));
    return *this;
  }

  operator GLuint() const { return handle; }

  ~Handle() {
    if (handle != 0) {
      allocator.free(handle);
    }
  }

private:
  GLuint handle;
  ALLOCATOR allocator{};

  void move(Handle &&rHandle) {
    this->handle = rHandle.handle;
    rHandle.handle = 0;
  }
};

template <typename ALLOCATOR>
std::ostream &operator<<(std::ostream &stream,
                         const Handle<ALLOCATOR> &handle) {
  stream << "Allocator:" << typeid(ALLOCATOR).name()
         << "\tHandle:" << (GLuint)handle;
  return stream;
}

struct ProgramAllocator {
public:
  GLuint alloc() { return glCreateProgram(); }
  void free(GLuint handle) { glDeleteProgram(handle); }
};

struct ShaderAllocator {
public:
  GLuint alloc(GLint TYPE) { return glCreateShader(TYPE); }
  void free(GLuint handle) { glDeleteShader(handle); }
};

struct BufferAllocator {
public:
  GLuint alloc() {
    GLuint h;
    glCreateBuffers(1, &h);
    return h;
  };
  void free(GLuint handle) { glDeleteBuffers(1, &handle); }
};

struct VertexArrayAllocator {
public:
  GLuint alloc() {
    GLuint h;
    glCreateVertexArrays(1, &h);
    return h;
  }

  void free(GLuint handle) { glDeleteVertexArrays(1, &handle); }
};

struct TextureAllocator {
  GLuint alloc(GLenum target) {
    GLuint h;
    glCreateTextures(target, 1, &h);
    return h;
  }
  void free(GLuint handle) { glDeleteTextures(1, &handle); }
};

using ShaderHandle = Handle<ShaderAllocator>;
using ShaderProgramHandle = Handle<ProgramAllocator>;
using BufferHandle = Handle<BufferAllocator>;
using VertexArrayHandle = Handle<VertexArrayAllocator>;
using TextureHandle = Handle<TextureAllocator>;
