#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "handle.hpp"
#include "traits.hpp"

class Texture : public glObject, public Bindable {
public:
  Texture(GLenum target) : handle{target}, target{target} {}

  virtual const GLuint getHandle() override { return handle; }

  virtual void bind() override { glBindTexture(target, handle); }
  virtual void unbind() override { glBindTexture(target, 0); }

  void textureParameteri(GLenum pname, GLenum param) {
    glTextureParameteri(handle, pname, param);
  }

  void textureStorage2D(GLsizei levels, GLenum internalformat, GLsizei width,
                        GLsizei height) {
    glTextureStorage2D(handle, levels, internalformat, width, height);
  }

  void textureSubImage2D(GLint level, GLint xoffset, GLint yoffset,
                         GLsizei width, GLsizei height, GLenum format,
                         GLenum type, const void *pixels) {
    glTextureSubImage2D(handle, level, xoffset, yoffset, width, height, format,
                        type, pixels);
  }

  void getnerateTextureMipmap() { glGenerateTextureMipmap(handle); }

  void bindTextureUnit(GLuint unit) { glBindTextureUnit(unit, handle); }

  void pixelStorei(GLenum pname, GLint param) { glPixelStorei(pname, param); }

private:
  TextureHandle handle;
  const GLenum target;
};
