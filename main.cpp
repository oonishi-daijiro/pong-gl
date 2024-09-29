#include <chrono>
#include <codecvt>
#include <cstddef>
#include <cwchar>
#include <glm/fwd.hpp>
#include <initializer_list>
#include <ios>
#include <iostream>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "debug.hpp"
#include "pong.hpp"

#define WIDTH 1080
#define HEIGHT 720

GLFWwindow *const Init() {
  atexit(glfwTerminate);

  if (glfwInit() == GL_FALSE) {
    std::cerr << "Can't initialize GLFW" << std::endl;
    return GL_FALSE;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 16);

  GLFWwindow *const window = Window::create(WIDTH, HEIGHT, "PONG", NULL, NULL);

  if (window == NULL) {
    std::cerr << "Can't create GLFW window." << std::endl;
    return GL_FALSE;
  }

  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    std::cout << "can't initialize glew" << std::endl;
    return GL_FALSE;
  }

  glEnable(GL_ALPHA_TEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glfwSwapInterval(1);

  // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  // glEnable(GL_DEBUG_OUTPUT);

  glDebugMessageCallback(openglOnError, NULL);
  glfwSetErrorCallback(glfwOnError);

  Ft2Wrap::freetype2::init();

  return window;
}

int main(int argc, char **argv) {
  GLFWwindow *const window = Init();

  if (window == GL_FALSE) {
    return 1;
  };
  if (argc < 3) {
    std::cout << "Please input player name" << std::endl;
    return 1;
  }
  std::string lp{argv[1]};
  std::string rp{argv[2]};

  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

  Pong pongGame{converter.from_bytes(lp.data()),
                converter.from_bytes(rp.data())};

  glfwSetWindowSizeCallback(window, Window::onResize);

  while (glfwWindowShouldClose(window) == GL_FALSE) {

    glClearColor(0.9, 0.9, 0.9, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    pongGame.update();
    pongGame.draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  return 0;
}
