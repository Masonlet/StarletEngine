#include "StarletEngine/engine.hpp"
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

    renderFrame();
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
  Camera* cam{ nullptr };
  if (!sceneManager.getScene().getObjectByIndex<Camera>(cameraController.current, cam)) return;
  cameraController.adjustFov(*cam, static_cast<float>(-yOffset));
}

void Engine::renderFrame() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Camera* cam{ getActiveCamera() };
  if (!cam) return;

  cameraController.update(*cam, inputManager, deltaTime);

  const std::map<std::string, Light>& lights = sceneManager.getScene().getObjects<Light>();
  const std::map<std::string, Model>& models = sceneManager.getScene().getObjects<Model>();

  renderer.updateCameraUniforms(cam->pos, Mat4::lookAt(cam->pos, cam->front), Mat4::perspective(cam->fov, windowManager.getAspect(), cam->nearPlane, cam->farPlane));
  renderer.updateLightUniforms(lights);

  renderer.drawModels(models, cam->pos);

  Model* skybox{ nullptr };
  if (sceneManager.getScene().getObjectByName(std::string("skybox"), skybox))
    renderer.drawSkybox(*skybox, cam->pos);

  glBindVertexArray(0);
}

Camera* Engine::getActiveCamera() {
  Camera* cam{ nullptr };
  if (!sceneManager.getScene().getObjectByIndex<Camera>(cameraController.current, cam)) {
    error("Engine", "getActiveCamera", "No active camera found for selected camera");
    return nullptr;
  }
  return cam;
}