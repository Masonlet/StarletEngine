#include "StarletEngine/engine.hpp"

#include "StarletScene/components/model.hpp"
#include "StarletScene/components/textureData.hpp"

#include "StarletScene/systems/cameraMoveSystem.hpp"
#include "StarletScene/systems/cameraLookSystem.hpp"
#include "StarletScene/systems/cameraFovSystem.hpp"
#include "StarletScene/systems/velocitySystem.hpp"

#include <GLFW/glfw3.h>

Engine::Engine() : renderer(shaderManager, meshManager, textureManager), resourceLoader(meshManager, textureManager) {}

void Engine::setAssetPaths(const std::string& path) {
  shaderManager.setBasePath((path + "/shaders/").c_str());
  meshManager.setBasePath((path + "/models/").c_str());
  textureManager.setBasePath((path + "/textures/").c_str());
  sceneManager.setBasePath((path + "/scenes/").c_str());
}

bool Engine::initialize(const unsigned int width, const unsigned int height, const char* title) {
  debugLog("Engine", "initialize", "Start time: " + std::to_string(glfwGetTime()));

  if (!windowManager.createWindow(width, height, title)) return false;
  windowManager.setWindowPointer(this);

	debugLog("ShaderManager", "createProgramFromPaths", "Start time: " + std::to_string(glfwGetTime()));
  if (!shaderManager.createProgramFromPaths("shader1", "vertex_shader.glsl", "fragment_shader.glsl"))
    return error("Engine", "initialize", "Failed to create shader program from file");
	debugLog("ShaderManager", "createProgramFromPaths", "Finish time: " + std::to_string(glfwGetTime()));

  debugLog("Renderer", "initialize", "Start time: " + std::to_string(glfwGetTime()));
  if (!renderer.initialize()) 
    return error("Engine", "initialize", "Failed to setup shaders for renderer");
  debugLog("Renderer", "initialize", "Finish time: " + std::to_string(glfwGetTime()));

  return (renderer.getProgram() == 0)
    ? error("Engine", "initialize", "No active shader program set after initializing")
		: debugLog("Engine", "initialize", "Finish Time: " + std::to_string(glfwGetTime()));
}

bool Engine::loadScene(const std::string& sceneIn) {
  debugLog("Engine", "loadScene", "Start time: " + std::to_string(glfwGetTime()));

  if (sceneIn.empty()) {
    if (!sceneManager.loadTxtScene("EmptyScene.txt"))
      return error("Engine", "loadScene", "No scene loaded and failed to load Default \"EmptyScene\"");
  }
  else if (!sceneManager.loadTxtScene(sceneIn + ".txt"))
    return error("Engine", "loadScene", "Failed to load scene: " + sceneIn);

  debugLog("ResourceLoader", "loadMeshes", "Start time: " + std::to_string(glfwGetTime()));
  if(!resourceLoader.loadMeshes(sceneManager.getScene().getComponentsOfType<Model>()))
		return error("Engine", "loadMeshes", "Failed to load meshes for scene: " + sceneIn);
  debugLog("ResourceLoader", "loadMeshes", "Finish time: " + std::to_string(glfwGetTime()));

  debugLog("ResourceLoader", "loadTextures", "Start time: " + std::to_string(glfwGetTime()));
  if(!resourceLoader.loadTextures(sceneManager.getScene().getComponentsOfType<TextureData>()))
		return error("Engine", "loadTextures", "Failed to load textures for scene: " + sceneIn);
  debugLog("ResourceLoader", "loadTextures", "Finish time: " + std::to_string(glfwGetTime()));

  debugLog("ResourceLoader", "processPrimitives", "Start time: " + std::to_string(glfwGetTime()));
  if(!resourceLoader.processPrimitives(sceneManager))
		return error("Engine", "processPrimitives", "Failed to process primitives for scene: " + sceneIn);
  debugLog("ResourceLoader", "processPrimitives", "Finish time: " + std::to_string(glfwGetTime()));

  debugLog("ResourceLoader", "processGrids", "Start time: " + std::to_string(glfwGetTime()));
  if(!resourceLoader.processGrids(sceneManager))
		return error("Engine", "processGrids", "Failed to process grids for scene: " + sceneIn);
  debugLog("ResourceLoader", "processGrids", "Finish time: " + std::to_string(glfwGetTime()));

  sceneManager.getScene().registerSystem(std::make_unique<CameraMoveSystem>());
  sceneManager.getScene().registerSystem(std::make_unique<CameraLookSystem>());
  sceneManager.getScene().registerSystem(std::make_unique<CameraFovSystem>());
  sceneManager.getScene().registerSystem(std::make_unique<VelocitySystem>());

  return debugLog("Engine", "loadScene", "Finish Time: " + std::to_string(glfwGetTime()));
}

void Engine::run() {
  windowManager.switchActiveWindowVisibility();

  while (!windowManager.shouldClose()) {
    const float deltaTime = timer.tick();

    inputManager.reset();
    windowManager.pollEvents();
    inputManager.updateMousePosition(windowManager.getGLFWwindow());

    handleKeyEvents(inputManager.consumeKeyEvents());
    handleButtonEvents(inputManager.consumeButtonEvents());

    sceneManager.getScene().updateSystems(inputManager, deltaTime);
    renderer.renderFrame(sceneManager.getScene(), windowManager.getAspect());

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

void Engine::handleButtonEvents(const std::vector<MouseButtonEvent>& buttonEvents) {
  for (const MouseButtonEvent event : buttonEvents) {
    std::string buttonName;
    switch (event.button) {
    case GLFW_MOUSE_BUTTON_LEFT:   buttonName = "Left"; break;
    case GLFW_MOUSE_BUTTON_RIGHT:  buttonName = "Right"; break;
    case GLFW_MOUSE_BUTTON_MIDDLE: buttonName = "Middle"; break;
    case GLFW_MOUSE_BUTTON_4:      buttonName = "Side_Forward"; break;
    case GLFW_MOUSE_BUTTON_5:      buttonName = "Side_Backward"; break;
    default: buttonName = "Unknown"; break;
    }

    std::string actionName;
    switch (event.action) {
    case GLFW_PRESS:   actionName = "Pressed"; break;
    case GLFW_RELEASE: actionName = "Released"; break;
    default: return;
    }

    debugLog("Input", "Mouse", "Button " + buttonName + " " + actionName);
  }
}