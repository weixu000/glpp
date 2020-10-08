#include "common.hpp"

#include <glpp/gl.h>

#include <iostream>

namespace {
void ErrorCallback(int error, const char* description) {
  std::cerr << "Error: " << description << std::endl;
  exit(EXIT_FAILURE);
}
}  // namespace

GLFWwindow* SetupGL(const char* title) {
  if (!glfwInit()) exit(EXIT_FAILURE);
  const auto window = glfwCreateWindow(640, 480, title, nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetErrorCallback(ErrorCallback);

  glfwMakeContextCurrent(window);
  if (!gladLoadGL()) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSwapInterval(1);
  return window;
}