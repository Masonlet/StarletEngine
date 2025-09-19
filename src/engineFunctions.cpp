#include "StarletEngine/engine.hpp"
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

Camera* Engine::getActiveCamera() {
  Camera* cam{ nullptr };
  if (!sceneManager.getScene().getObjectByIndex<Camera>(cameraController.current, cam)) {
    error("Engine", "getActiveCamera", "No active camera found for selected camera");
    return nullptr;
  }
  return cam;
}

void Engine::toggleCursorLock() {
  bool wasLocked = windowManager.switchCursorLock();
  inputManager.setCursorLocked(!wasLocked);
}

void Engine::toggleWireframe() {
  wireframe = !wireframe;
  glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
}