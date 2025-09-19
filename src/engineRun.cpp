#include "StarletEngine/engine.hpp"
#include "StarletParsers/utils/log.hpp"
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
  constexpr float smoothingFactor = 0.9f;

  if (rawDelta > maxDelta) {
    debugLog("Engine", "Tick", "deltaTime clamped to " + std::to_string(maxDelta) + " (was " + std::to_string(rawDelta) + ")", true);
    rawDelta = maxDelta;
  }

  lastTime = currentTime;
  deltaTime = smoothingFactor * deltaTime + (1.0f - smoothingFactor) * rawDelta;
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