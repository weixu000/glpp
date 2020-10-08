#include "common.hpp"

#include <glpp/gl.h>

#include <iostream>

namespace {
void ErrorCallback(int error, const char* description) {
  std::cerr << "Error: " << description << std::endl;
  exit(EXIT_FAILURE);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void FramebufferSizeCallback(GLFWwindow* window, int w, int h) {
  width = w;
  height = h;
  glViewport(0, 0, width, height);
}
}  // namespace

int width, height;

GLFWwindow* SetupGL(const char* title) {
  if (!glfwInit()) exit(EXIT_FAILURE);

  width = 640;
  height = 480;
  const auto window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetErrorCallback(ErrorCallback);
  glfwSetKeyCallback(window, KeyCallback);
  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

  glfwMakeContextCurrent(window);
  if (!gladLoadGL()) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSwapInterval(1);
  return window;
}