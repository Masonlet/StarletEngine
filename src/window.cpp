#include "StarletEngine/window.hpp"
#include "StarletParser/utils/log.hpp"

#include "StarletControls/inputManager.hpp"

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
	if (userPointer == nullptr) error("Window", "setWindowPointer", "User pointer cannot be null.");
	else if (window) glfwSetWindowUserPointer(window, userPointer);
}
void Window::setCurrentWindow() const {
	if (window) glfwMakeContextCurrent(window);
}

void Window::updateViewport(const unsigned int widthIn, const unsigned int heightIn) {
	if (window) {
		width = widthIn;
		height = heightIn;
		glViewport(0, 0, width, height);
	}
}

bool Window::switchActiveWindowVisibility() {
	if (!window) return error("Window", "switchActiveWindowVisibility", "No active window to switch visibility.");

	const int isVisible = (glfwGetWindowAttrib(window, GLFW_VISIBLE) == GLFW_TRUE) ? GLFW_FALSE : GLFW_TRUE; 
	(isVisible == GLFW_TRUE) ? glfwShowWindow(window) : glfwHideWindow(window);

	return (isVisible == GLFW_TRUE)
		? debugLog("Window", "switchWindowVisibility", "Window shown") 
		: debugLog("Window", "switchWindowVisibility", "Window hidden", false);
}
bool Window::switchCursorLock() {
	if (!window) return error("Window", "switchCursorLock", "No active window to switch cursor lock.");

	const int cursorMode = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
	glfwSetInputMode(window, GLFW_CURSOR, cursorMode);

	return (cursorMode == GLFW_CURSOR_DISABLED) 
		? debugLog("Window", "switchCursorLock", "Cursor locked") 
		: debugLog("Window", "switchCursorLock", "Cursor unlocked", false);
}


