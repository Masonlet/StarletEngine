#include "StarletEngine/engine.hpp"
#include "StarletParsers/utils/log.hpp"
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

void Engine::run() {
  windowManager.switchActiveWindowVisibility();
  const Window* window = windowManager.getWindow();

  while (!window->shouldClose()) {
    updateTime(static_cast<float>(glfwGetTime()));

    inputManager.clear();
    window->pollEvents();
    inputManager.update(window->getGLFWwindow());
    handleKeyEvents(inputManager.consumeKeyEvents());
    handleScrollEvents(inputManager.consumeScrollX(), inputManager.consumeScrollY());

    renderFrame();
    window->swapBuffers();
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
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(windowManager.getWindow()->getGLFWwindow(), GLFW_TRUE);
      break;

#ifndef NDEBUG
    case GLFW_KEY_P:
      wireframe = !wireframe;
      glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
      break;

    case GLFW_KEY_C: {
      GLFWwindow* window = windowManager.getWindow()->getGLFWwindow();
      bool locked = glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED;
      glfwSetInputMode(window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
      inputManager.setCursorLocked(locked);
      break;
    }
#endif
    }
  }
}
void Engine::handleScrollEvents(double xOffset, double yOffset) {
  Camera* cam{ nullptr };
  if (!sceneManager.scene.getObjectByIndex<Camera>(cameraController.current, cam)) return;
  cameraController.adjustFov(*cam, static_cast<float>(-yOffset));
}

void Engine::renderFrame() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Camera* cam{ nullptr };
  if (!sceneManager.scene.getObjectByIndex<Camera>(cameraController.current, cam)) {
    error("Engine", "run", "No active camera found for selected camera");
    return;
  }

  cameraController.update(*cam, inputManager, deltaTime);
  renderer.updateCameraUniforms(cam->pos, Mat4::lookAt(cam->pos, cam->front), Mat4::perspective(cam->fov, windowManager.getWindow()->getAspect(), cam->nearPlane, cam->farPlane));
  renderer.updateLightUniforms(sceneManager.scene.getObjects<Light>());

  bool skyboxFound{ false };
  std::vector<const Model*> transparentInstances;

  for (const std::pair<const std::string, Model>& model : sceneManager.scene.getObjects<Model>()) {
    const Model& instance = model.second;
    if (instance.name == "skybox") {
      skyboxFound = true;
      continue;
    }

    if (instance.colour.w >= 1.0f) renderer.drawModel(meshManager, textureManager, instance);
    else                           transparentInstances.push_back(&instance);
  }

  for (size_t i = 0; i < transparentInstances.size(); ++i) {
    for (size_t j = 0; j < transparentInstances.size() - i - 1; ++j) {
      const Vec4& a = transparentInstances[j]->transform.pos;
      const Vec4& b = transparentInstances[j + 1]->transform.pos;

      const Vec3 eye = cam->pos;
      float distA = (a.x - eye.x) * (a.x - eye.x) + (a.y - eye.y) * (a.y - eye.y) + (a.z - eye.z) * (a.z - eye.z);
      float distB = (b.x - eye.x) * (b.x - eye.x) + (b.y - eye.y) * (b.y - eye.y) + (b.z - eye.z) * (b.z - eye.z);

      if (distA < distB) {
        const Model* temp = transparentInstances[j];
        transparentInstances[j] = transparentInstances[j + 1];
        transparentInstances[j + 1] = temp;
      }
    }
  }

  for (const Model* instance : transparentInstances)
    renderer.drawModel(meshManager, textureManager, *instance);

  if (skyboxFound) {
    Model* skybox{ nullptr };
    sceneManager.scene.getObjectByName(std::string("skybox"), skybox);

    renderer.bindSkyboxTexture(textureManager.getTextureID(skybox->name));
    renderer.setModelIsSkybox(true);
    skybox->isVisible = true;
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glDepthMask(GL_FALSE);
    skybox->transform.pos = { cam->pos, 0.0f };
    renderer.drawModel(meshManager, textureManager, *skybox);
    glDepthMask(GL_TRUE);
    glCullFace(GL_BACK);
    skybox->isVisible = false;
    renderer.setModelIsSkybox(false);
  }

  glBindVertexArray(0);
}