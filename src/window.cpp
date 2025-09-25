#include "StarletEngine/window.hpp"

#include "StarletControls/inputManager.hpp"

#include "StarletParsers/utils/log.hpp"

#include <GLFW/glfw3.h>
#include <cstdio>

Window::~Window() {
	if (window) {
		glfwDestroyWindow(window);
		window = nullptr;
	}
}

bool Window::createWindow(const unsigned int widthIn, const unsigned int heightIn, const char* title) {
	window = glfwCreateWindow(widthIn, heightIn, title, nullptr, nullptr);
	if (!window) return error("Window", "createWindow", "Failed to create GLFW window");

	width = widthIn;
	height = heightIn;
	return debugLog("Window", "createWindow", "Created window: " + std::string(title) + " - " + std::to_string(width) + " x " + std::to_string(height));
}
bool Window::shouldClose() const {
	return window ? glfwWindowShouldClose(window) : true;
}

void Window::pollEvents() const {
	if (window) glfwPollEvents();
}
void Window::swapBuffers() const {
	if (window) glfwSwapBuffers(window);
}
void Window::requestClose() const {
	if (window) glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Window::setWindowPointer(void* userPointer) const {
	if (window) glfwSetWindowUserPointer(window, userPointer);
}
void Window::setCurrentWindow() const {
	if (window) glfwMakeContextCurrent(window);
}

void Window::updateViewport(const unsigned int widthIn, const unsigned int heightIn) {
	width = widthIn;
	height = heightIn;
	glViewport(0, 0, width, height);
}

bool Window::switchActiveWindowVisibility() {
	if (!window) return error("WindowManager", "switchActiveWindowVisibility", "No active window to switch visibility.");

	glfwGetWindowAttrib(window, GLFW_VISIBLE) ? glfwHideWindow(window) : glfwShowWindow(window);

	return glfwGetWindowAttrib(window, GLFW_VISIBLE);
}
bool Window::switchCursorLock() {
	if (!window) return error("WindowManager", "switchCursorLock", "No active window to switch cursor lock.");

	const bool locked = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
	const bool newLock = !locked;
	glfwSetInputMode(window, GLFW_CURSOR, newLock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

	debugLog("WindowManager", "toggleCursorLock", std::string("Cursor ") + (newLock ? "locked" : "unlocked"));
	return newLock;
}


