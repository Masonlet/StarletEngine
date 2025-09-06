#include <glad/glad.h> 
#define GLFW_INCLUDE_NONE 
#include <GLFW/glfw3.h>
#include "StarletEngine/window.hpp"
#include "StarletParsers/utils/log.hpp"
#include <cstdio>

Window::~Window() {
	if (window) {
		glfwDestroyWindow(window);
		window = nullptr;
	}
}

bool Window::createWindow(const unsigned int widthIn, const unsigned int heightIn, const char* title) {
	width = widthIn;
	height = heightIn;
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window) return error("Window", "createWindow", "Failed to create GLFW window");
	return true;
}

void Window::pollEvents() const { 
	glfwPollEvents(); 
}
void Window::swapBuffers() const { 
	glfwSwapBuffers(window); 
}

bool Window::shouldClose() const { 
	return glfwWindowShouldClose(window); 
}
void Window::updateViewport(const unsigned int widthIn, const unsigned int heightIn) {
	width = widthIn;
	height = heightIn;
	glViewport(0, 0, width, height);
}