#include "StarletEngine/engine.hpp"

#include "StarletScene/components/model.hpp"
#include "StarletScene/components/textureData.hpp"

#include "StarletScene/systems/cameraMoveSystem.hpp"
#include "StarletScene/systems/cameraLookSystem.hpp"
#include "StarletScene/systems/cameraFovSystem.hpp"
#include "StarletScene/systems/velocitySystem.hpp"

#include <GLFW/glfw3.h>

Engine::Engine() : renderer(shaderManager, meshManager, textureManager), resourceLoader(shaderManager, meshManager, textureManager) {}

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
      return error("Engine", "loadScene", "No scene loaded and failed to load Default \"EmptyScene\"");
  }
  else if (!sceneManager.loadTxtScene(sceneIn + ".txt"))
    return error("Engine", "loadScene", "Failed to load scene: " + sceneIn);

  if (renderer.getProgram() == 0) return error("Engine", "loadScene", "No active shader program set after loading scene");

  bool ok = true;

  debugLog("ResourceLoader", "loadMeshes", "Start time: " + std::to_string(glfwGetTime()));
  if(!resourceLoader.loadMeshes(sceneManager.getScene().getComponentsOfType<Model>()))
		return error("Engine", "loadMeshes", "Failed to load meshes for scene: " + sceneIn);
  debugLog("ResourceLoader", "loadMeshes", "Finish time: " + std::to_string(glfwGetTime()));

  debugLog("ResourceLoader", "loadTextures", "Start time: " + std::to_string(glfwGetTime()));
  if(!resourceLoader.loadTextures(sceneManager.getScene().getComponentsOfType<TextureData>()))
		return error("Engine", "loadTextures", "Failed to load textures for scene: " + sceneIn);
  debugLog("ResourceLoader", "loadTextures", "Finish time: " + std::to_string(glfwGetTime()));

  debugLog("ResourceLoader", "processTextureConnections", "Start time: " + std::to_string(glfwGetTime()));
  if(!resourceLoader.processTextureConnections(sceneManager))
		return error("Engine", "processTextureConnections", "Failed to process texture connections for scene: " + sceneIn);
  debugLog("ResourceLoader", "processTextureConnections", "Finish time: " + std::to_string(glfwGetTime()));

  debugLog("ResourceLoader", "processPrimitives", "Start time: " + std::to_string(glfwGetTime()));
  if(!resourceLoader.processPrimitives(sceneManager))
		return error("Engine", "processPrimitives", "Failed to process primitives for scene: " + sceneIn);
  debugLog("ResourceLoader", "processPrimitives", "Finish time: " + std::to_string(glfwGetTime()));

  debugLog("ResourceLoader", "processGrids", "Start time: " + std::to_string(glfwGetTime()));
  if(!resourceLoader.processGrids(sceneManager))
		return error("Engine", "processGrids", "Failed to process grids for scene: " + sceneIn);
  debugLog("ResourceLoader", "processGrids", "Finish time: " + std::to_string(glfwGetTime()));

  debugLog("ResourceLoader", "updateLightUniforms", "Start time: " + std::to_string(glfwGetTime()));
  renderer.updateLightUniforms(sceneManager.getScene());
  debugLog("ResourceLoader", "updateLightUniforms", "Finish time: " + std::to_string(glfwGetTime()));

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