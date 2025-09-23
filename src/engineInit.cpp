#include "StarletEngine/engine.hpp"

#include <GLFW/glfw3.h>

void Engine::setAssetPaths(const std::string& path) {
  renderer.setAssetPaths(path.c_str());
  sceneManager.setBasePath((path + "/scenes/").c_str());
}

bool Engine::initialize(const unsigned int width, const unsigned int height, const char* title) {
  debugLog("Engine", "initialize", "Start time: " + std::to_string(glfwGetTime()), true);

  if (!windowManager.createWindow(width, height, title)) return false;
  windowManager.setWindowPointer(this);

  if (!setupShaders()) return false;
  setupGLState();

  return debugLog("Engine", "initialize", "Finish time: " + std::to_string(glfwGetTime()), true);
}

bool Engine::setupShaders() {
  debugLog("Engine", "setupShaders", "Start time: " + std::to_string(glfwGetTime()), true);

  if (!renderer.createProgramFromPaths("shader1", "vertex_shader.glsl", "fragment_shader.glsl"))
    return error("Engine", "setupShaders", "Failed to create shader program from file");

  if (!renderer.setProgram(renderer.getProgramID("shader1")))
    return error("Engine", "setupShaders", "Failed to set program to shader1");

  if(!renderer.cacheUniformLocations())
		return error("Engine", "setupShaders", "Failed to cache uniform locations");

  return debugLog("Engine", "setupShaders", "Finish time: " + std::to_string(glfwGetTime()), true);
}
void Engine::setupGLState() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}
