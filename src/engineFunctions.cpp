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


