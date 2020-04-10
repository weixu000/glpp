#include "common.hpp"

#include <iostream>

void ErrorCallback(int error, const char* description) {
  std::cerr << "Error: " << description << std::endl;
  exit(EXIT_FAILURE);
}

GLFWwindow* SetupGL() {
  if (!glfwInit()) exit(EXIT_FAILURE);
  const auto window =
      glfwCreateWindow(640, 480, "glpp example", nullptr, nullptr);
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