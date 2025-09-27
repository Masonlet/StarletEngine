#pragma once

struct GLFWwindow;

void error_callback(const int error, const char* description);

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height);

void key_callback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods);

void scroll_callback(GLFWwindow* window, const double xoffset, const double yoffset);
void mouse_button_callback(GLFWwindow* window, const int button, const int action, const int mods);