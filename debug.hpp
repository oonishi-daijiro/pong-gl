#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

inline const char *openglSrc2str(GLenum source) {
  switch (source) {
  case GL_DEBUG_SOURCE_API:
    return "OpenGL";
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    return "Window system";
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    return "Third party";
  case GL_DEBUG_SOURCE_APPLICATION:
    return "Application";
  case GL_DEBUG_SOURCE_OTHER:
    return "Other";
  default:
    return "Unknown";
  }
}

inline const char *openglDebugType2str(GLenum type) {
  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    return "Error";
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    return "Deprecated behavior";
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    return "Undefined behavior";
  case GL_DEBUG_TYPE_PORTABILITY:
    return "Portability";
  case GL_DEBUG_TYPE_PERFORMANCE:
    return "Performance";
  case GL_DEBUG_TYPE_MARKER:
    return "Marker";
  case GL_DEBUG_TYPE_PUSH_GROUP:
    return "Push group";
  case GL_DEBUG_TYPE_POP_GROUP:
    return "Pop group";
  case GL_DEBUG_TYPE_OTHER:
    return "Other";
  default:
    return "Unknown";
  }
}
inline const char *openglSeverity2str(GLenum severity) {
  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    return "High";
  case GL_DEBUG_SEVERITY_MEDIUM:
    return "Medium";
  case GL_DEBUG_SEVERITY_LOW:
    return "Low";
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    return "Notification";
  default:
    return "Unknown";
  }
}

inline void APIENTRY openglOnError(GLenum source, GLenum type, GLuint id,
                                   GLenum severity, GLsizei length,
                                   const GLchar *message, const void *param) {
  const char *source_str = openglSrc2str(source);
  const char *type_str = openglDebugType2str(type);
  const char *severity_str = openglSeverity2str(severity);

  std::cerr << "Source" << source_str << "Type:" << type_str
            << "Severity:" << severity_str << "ID:" << id;
  std::cerr << "error code\t" << id << message << std::endl;
}

inline void glfwOnError(int error_code, const char *description) {
  std::cout << "ErrorCode:" << error_code << "detail:" << description
            << std::endl;
}
