#pragma once

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string_view>

class Window {
private:
  static inline int width;
  static inline int height;

public:
  static inline GLFWwindow *create(int width, int height,
                                   std::string_view title, GLFWmonitor *monitor,
                                   GLFWwindow *share) {
    setSize(width, height);
    return glfwCreateWindow(width, height, title.data(), monitor, share);
  }
  static inline void onResize(GLFWwindow *const window, int newWidth,
                              int newHeight) {
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    glOrtho(0.0, width, height, 0.0, -1.0, 1.0);
    width = newWidth;
    height = newHeight;
  }

  static inline void setSize(int width, int height) {
    Window::width = width;
    Window::height = height;
  }

  static inline float getAspect() {
    return static_cast<float>(width) / static_cast<float>(height);
  }
  static inline std::tuple<int, int> getSize() { return {width, height}; }
};
