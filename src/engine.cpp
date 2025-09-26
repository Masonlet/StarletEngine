#include "StarletEngine/engine.hpp"

#include "StarletScene/components/transform.hpp"
#include "StarletScene/components/model.hpp"
#include "StarletScene/components/light.hpp"
#include "StarletScene/components/camera.hpp"
#include "StarletScene/components/grid.hpp"
#include "StarletScene/components/textureData.hpp"
#include "StarletScene/components/textureConnection.hpp"
#include "StarletScene/components/primitive.hpp"

#include "StarletScene/systems/cameraMoveSystem.hpp"
#include "StarletScene/systems/cameraLookSystem.hpp"
#include "StarletScene/systems/cameraFovSystem.hpp"
#include "StarletScene/systems/velocitySystem.hpp"

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

bool Engine::loadScene(const std::string& sceneIn) {
  debugLog("Engine", "loadScene", "Start time: " + std::to_string(glfwGetTime()));

  if (sceneIn.empty()) {
    if (!sceneManager.loadTxtScene("EmptyScene.txt"))
      return error("Engine", "loadSceneMeshes", "No scene loaded and failed to load Default \"EmptyScene\"");
  }
  else if (!sceneManager.loadTxtScene(sceneIn + ".txt"))
    return error("Engine", "setScene", "Failed to load scene: " + sceneIn);

  if (renderer.getProgram() == 0) return error("Engine", "loadScene", "No active shader program set after loading scene");

  if (!resourceLoader) resourceLoader = std::make_unique<ResourceLoader>(renderer);

  bool ok = true;
  ok &= resourceLoader.get()->loadMeshes(sceneManager.getScene().getComponentsOfType<Model>());
  resourceLoader.get()->updateLighting(sceneManager.getScene());
  ok &= resourceLoader.get()->loadTextures(sceneManager.getScene().getComponentsOfType<TextureData>());
  ok &= resourceLoader.get()->processTextureConnections(sceneManager);
  ok &= resourceLoader.get()->processPrimitives(sceneManager);
  ok &= resourceLoader.get()->processGrids(sceneManager);

  sceneManager.getScene().registerSystem(std::make_unique<CameraMoveSystem>());
  sceneManager.getScene().registerSystem(std::make_unique<CameraLookSystem>());
  sceneManager.getScene().registerSystem(std::make_unique<CameraFovSystem>());
  sceneManager.getScene().registerSystem(std::make_unique<VelocitySystem>());

  return ok
    ? debugLog("Engine", "loadScene", "Finish Time: " + std::to_string(glfwGetTime()))
    : error("Engine", "loadScene", "Failed to load scene: " + sceneIn);;
}

void Engine::run() {
  windowManager.switchActiveWindowVisibility();

  while (!windowManager.shouldClose()) {
    float deltaTime = timer.tick();

    inputManager.clear();
    windowManager.pollEvents();
    inputManager.update(windowManager.getGLFWwindow());

    handleKeyEvents(inputManager.consumeKeyEvents());

    Scene& scene{ sceneManager.getScene() };
    scene.updateSystems(inputManager, deltaTime);
    renderer.renderFrame(scene, windowManager.getAspect());

    windowManager.swapBuffers();
  }
}

void Engine::handleKeyEvents(const std::vector<KeyEvent>& keyEvents) {
  for (const KeyEvent event : keyEvents) {
    if (event.action != GLFW_PRESS) continue;

    switch (event.key) {
    case GLFW_KEY_ESCAPE: windowManager.requestClose(); break;

#ifndef NDEBUG
    case GLFW_KEY_P: toggleWireframe();  break;
    case GLFW_KEY_C: toggleCursorLock(); break;
#endif
    }
  }
}