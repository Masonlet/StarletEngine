#include "StarletEngine/engine.hpp"

#include "StarletScene/components/model.hpp"
#include "StarletScene/components/light.hpp"
#include "StarletScene/components/camera.hpp"
#include "StarletScene/components/grid.hpp"
#include "StarletScene/components/textureData.hpp"
#include "StarletScene/components/textureConnection.hpp"
#include "StarletScene/components/primitive.hpp"

#include "StarletParsers/utils/log.hpp"

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

void Engine::run() {
  windowManager.switchActiveWindowVisibility();

  while (!windowManager.shouldClose()) {
    updateTime(static_cast<float>(glfwGetTime()));

    inputManager.clear();
    windowManager.pollEvents();
    windowManager.updateInput(inputManager);
    handleKeyEvents(inputManager.consumeKeyEvents());
    handleScrollEvents(inputManager.consumeScrollX(), inputManager.consumeScrollY());

    Camera* cam{ getActiveCamera() };
    if (!cam) {
      error("Engine", "renderFrame", "No active camera while rendering!");
      return;
    }

		Scene& scene{ sceneManager.getScene() };
    cameraController.update(*cam, inputManager, deltaTime);
    renderer.renderFrame(*cam, windowManager.getAspect(), scene.getComponentsOfType<Light>(), scene.getComponentsOfType<Model>(), *sceneManager.getScene().getComponentByName<Model>(std::string("skybox")));
    windowManager.swapBuffers();
  }
}

void Engine::updateTime(const float currentTime) {
  if (lastTime == 0.0f) {
    lastTime = currentTime;
    deltaTime = 0.0f;
    return;
  }

  float rawDelta = currentTime - lastTime;
  constexpr float maxDelta = 0.1f;

  lastTime = currentTime;
  deltaTime = (rawDelta > maxDelta) ? maxDelta : rawDelta;

  if (rawDelta > maxDelta) debugLog("Engine", "Tick", "deltaTime clamped to " + std::to_string(maxDelta) + " (was " + std::to_string(rawDelta) + ")", true);
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
void Engine::handleScrollEvents(double xOffset, double yOffset) {
  Camera* cam{ sceneManager.getScene().getComponentByIndex<Camera>(cameraController.current) };
  if (!cam) return;
  cameraController.adjustFov(*cam, static_cast<float>(-yOffset));
}

Camera* Engine::getActiveCamera() {
  Camera* cam{ sceneManager.getScene().getComponentByIndex<Camera>(cameraController.current) };
  if (!cam) {
    error("Engine", "getActiveCamera", "No active camera found for selected camera");
    return nullptr;
  }
  return cam;
}