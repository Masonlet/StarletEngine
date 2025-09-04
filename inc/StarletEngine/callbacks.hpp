#pragma once

#define GLFW_INCLUDE_NONE 
#include <GLFW/glfw3.h>

void error_callback(const int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);