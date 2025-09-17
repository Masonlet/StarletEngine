#pragma once

struct GLFWwindow;

void error_callback(const int error, const char* description);
void key_callback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods);
void framebuffer_size_callback(GLFWwindow* window, const int width, const int height);
void scroll_callback(GLFWwindow* window, const double xoffset, const double yoffset);