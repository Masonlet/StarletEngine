#include <glad/glad.h>
#include "StarletEngine/callbacks.hpp"
#include "StarletEngine/engine.hpp"
#include <stdio.h>

void error_callback(const int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
	if (!engine) return;
	
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE)	glfwSetWindowShouldClose(window, GLFW_TRUE);	
		
		if (key == GLFW_KEY_P) {
			engine->wireframe = !engine->wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, engine->wireframe ? GL_LINE : GL_FILL);
		}

		if (key == GLFW_KEY_C) {
			bool locked = glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED;
			glfwSetInputMode(window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
			engine->inputManager.setCursorLocked(locked);
		}

    //if(key == GLFW_KEY_L)
		//	engine->getSceneManager().saveTxtScene();

		if (key == GLFW_KEY_N) {
			if (mods & GLFW_MOD_SHIFT) engine->modelController.increment(engine->sceneManager.scene.getObjectCount<Model>());
			else										   engine->modelController.decrement();
		}

		if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) 
			engine->cameraController.setCamera(key - GLFW_KEY_0, engine->sceneManager.scene.getObjectCount<Camera>());
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	static int lastWidth = 0;
	static int lastHeight = 0;

	if (width == lastWidth && height == lastHeight)	return;

	lastWidth = width;
	lastHeight = height;

	Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
	if (engine) engine->windowManager.getWindow()->updateViewport(width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));

	Camera* cam{ nullptr };
	if (!engine->sceneManager.scene.getObjectByIndex<Camera>(engine->cameraController.current, cam)) {
		error("Engine", "Callbacks", "Failed to find active camera");
		return;
	}

	engine->cameraController.adjustFov(*cam, static_cast<float>(-yoffset * 2.0f));
}
