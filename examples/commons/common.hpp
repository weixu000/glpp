#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void ErrorCallback(int error, const char* description);

GLFWwindow* SetupGL();