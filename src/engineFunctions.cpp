#include "StarletEngine/engine.hpp"
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

Camera* Engine::getActiveCamera() {
  Camera* cam{ nullptr };
  if (!sceneManager.scene.getObjectByIndex<Camera>(cameraController.current, cam)) {
    error("Engine", "getActiveCamera", "No active camera found for selected camera");
    return nullptr;
  }
  return cam;
}

void Engine::toggleCursorLock() {
  GLFWwindow* window = windowManager.getWindow()->getGLFWwindow();
  bool wasLocked = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;

  glfwSetInputMode(window, GLFW_CURSOR, wasLocked ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
  inputManager.setCursorLocked(!wasLocked);
}

void Engine::toggleWireframe() {
  wireframe = !wireframe;
  glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
}