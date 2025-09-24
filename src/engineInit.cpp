#include "StarletEngine/engine.hpp"

#include <GLFW/glfw3.h>

void Engine::setAssetPaths(const std::string& path) {
  renderer.setAssetPaths(path.c_str());
  sceneManager.setBasePath((path + "/scenes/").c_str());
}

bool Engine::initialize(const unsigned int width, const unsigned int height, const char* title) {
  debugLog("Engine", "initialize", "Start time: " + std::to_string(glfwGetTime()));

  if (!windowManager.createWindow(width, height, title)) return false;
  windowManager.setWindowPointer(this);

  debugLog("Renderer", "setupShaders", "Start time: " + std::to_string(glfwGetTime()));
  if (!renderer.setupShaders()) return false;
  debugLog("Renderer", "setupShaders", "Finish time: " + std::to_string(glfwGetTime()));

  return debugLog("Engine", "initialize", "Finish time: " + std::to_string(glfwGetTime()));
}