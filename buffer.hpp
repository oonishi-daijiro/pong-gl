#pragma once

#include <array>
#include <bits/utility.h>
#include <initializer_list>
#include <iostream>
#include <span>
#include <thread>
#include <type_traits>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "handle.hpp"
#include "traits.hpp"

template <typename T> class ArrayBuffer : public glObject {
public:
  ArrayBuffer(size_t size, int usage = GL_DYNAMIC_STORAGE_BIT)
      : size{size}, usage(usage), handle{} {
    glNamedBufferStorage(handle, sizeof(T) * size, NULL, usage);
  };

  ArrayBuffer(std::span<T> buffer, const int usage = GL_DYNAMIC_STORAGE_BIT)
      : size{buffer.size()}, usage{usage}, handle{} {
    isEmpty = true;
    glNamedBufferStorage(handle, sizeof(T) * buffer.size(), buffer.data(),
                         usage);
  }

  ArrayBuffer(ArrayBuffer &&srcBuffer)
      : size{srcBuffer.size}, usage{srcBuffer.usage}, handle{} {
    this->handle = std::move(srcBuffer.handle);
  };

  ArrayBuffer(const ArrayBuffer &) = delete;

  ArrayBuffer &operator=(const ArrayBuffer &srcBuffer) {
    isEmpty = false;
    glCopyBufferSubData(srcBuffer.handle, handle, 0, 0, size * sizeof(T));
    this->size = srcBuffer.size;
    return *this;
  };

  ArrayBuffer &operator=(std::span<T> buffer) {
    if (buffer.size() == size) {
      subData(buffer.data());
    }
    return *this;
  }

  void namedBufferSubData(GLintptr offset, GLsizeiptr size,
                          const GLvoid *data) {
    isEmpty = false;
    glNamedBufferSubData(handle, offset, size, data);
  }

  virtual const GLuint getHandle() override { return handle; };
  bool empty() { return isEmpty; }

private:
  const size_t size;

  int usage;
  bool isEmpty = true;

  void subData(const T *p) {
    isEmpty = false;
    glNamedBufferSubData(handle, 0, sizeof(T) * size, p);
  }

  BufferHandle handle;
};

class VertexArray : Bindable, glObject {
public:
  VertexArray() {}

  template <typename... T> VertexArray(T &&...vertexBuffers) {
    int i = 0;
    for (auto &&e : {&vertexBuffers...}) {
      glVertexArrayVertexBuffer(handle, i, e->getHandle(), 0, 0);
      i++;
    }
  }

  void vertexArrayVertexBuffer(GLuint bindingindex, GLuint buffer,
                               GLintptr offset, GLsizei stride) {
    glVertexArrayVertexBuffer(handle, bindingindex, buffer, offset, stride);
  }

  void vertexArrayAttribBinding(GLuint attribIndex, GLuint bindingIndex) {
    glVertexArrayAttribBinding(handle, attribIndex, bindingIndex);
  }

  void vertexArrayAttribFormat(GLuint attribindex, GLint size, GLenum type,
                               GLboolean normalized, GLuint relativeoffset) {
    glVertexArrayAttribFormat(handle, attribindex, size, type, normalized,
                              relativeoffset);
  }

  void enableVertexArrayAttrib(GLuint attribIndex) {
    glEnableVertexArrayAttrib(handle, attribIndex);
  }

  void vertexArrayBindingDivisor(GLuint bindingindex, GLuint divisor) {
    glVertexArrayBindingDivisor(handle, bindingindex, divisor);
  }

  virtual const GLuint getHandle() override { return handle; }

  virtual void bind() override { glBindVertexArray(handle); }
  virtual void unbind() override { glBindVertexArray(0); }

private:
  VertexArrayHandle handle{};
  size_t size;
};
