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
    float deltaTime = timer.tick();
 
    inputManager.clear();
    windowManager.pollEvents();
    windowManager.updateInput(inputManager);

    handleKeyEvents(inputManager.consumeKeyEvents());
   
    Camera* cam{ getActiveCamera() };
    if (!cam) {
      error("Engine", "renderFrame", "No active camera while rendering!");
      return;
    }
    cameraController.update(*cam, inputManager, deltaTime);
    cameraController.adjustFov(*cam, static_cast<float>(-inputManager.consumeScrollY()));

		Scene& scene{ sceneManager.getScene() };
    renderer.renderFrame(*cam, windowManager.getAspect(), scene.getComponentsOfType<Light>(), scene.getComponentsOfType<Model>(), *scene.getComponentByName<Model>(std::string("skybox")));

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

Camera* Engine::getActiveCamera() {
  Camera* cam{ sceneManager.getScene().getComponentByIndex<Camera>(cameraController.current) };
  if (!cam) {
    error("Engine", "getActiveCamera", "No active camera found for selected camera");
    return nullptr;
  }
  return cam;
}