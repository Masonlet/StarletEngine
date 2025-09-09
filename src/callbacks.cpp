#include <glad/glad.h>
#include "StarletEngine/callbacks.hpp"
#include "StarletEngine/engine.hpp"
#include <stdio.h>

void error_callback(const int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}
void key_callback(GLFWwindow* window, int key, int scanCode, int action, int mods) {
	Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
	if (!engine) return;
	engine->inputManager.onKey(key, action, mods);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
	if (!engine) return;
	engine->windowManager.getWindow()->updateViewport(width, height);
}
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
	if (!engine) return;
	engine->inputManager.onScroll(xOffset, yOffset);
}
