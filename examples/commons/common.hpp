#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

extern int width, height;

GLFWwindow* SetupGL(const char* title);