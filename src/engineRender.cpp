#include "StarletEngine/engine.hpp"
#include <glad/glad.h> 

void Engine::renderFrame() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Camera* cam{ getActiveCamera() };
  if (!cam) return;

  updateEngineState(*cam);

  renderer.drawModels(sceneManager.getScene().getObjects<Model>(), cam->pos);

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

void Engine::updateEngineState(Camera& cam) {
  cameraController.update(cam, inputManager, deltaTime);
  renderer.updateCameraUniforms(cam.pos, Mat4::lookAt(cam.pos, cam.front), Mat4::perspective(cam.fov, windowManager.getAspect(), cam.nearPlane, cam.farPlane));
  renderer.updateLightUniforms(sceneManager.getScene().getObjects<Light>());
}