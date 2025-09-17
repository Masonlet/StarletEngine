#include "StarletEngine/engine.hpp"
#include <glad/glad.h> 

void Engine::renderFrame() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Camera* cam{ getActiveCamera() };
  if (!cam) return;

  updateEngineState(*cam);
  renderModels(cam->pos);
  renderSkybox(cam->pos);

  glBindVertexArray(0);
}

void Engine::updateEngineState(Camera& cam) {
  cameraController.update(cam, inputManager, deltaTime);
  renderer.updateCameraUniforms(cam.pos, Mat4::lookAt(cam.pos, cam.front), Mat4::perspective(cam.fov, windowManager.getAspect(), cam.nearPlane, cam.farPlane));
  renderer.updateLightUniforms(sceneManager.scene.getObjects<Light>());
}

void Engine::renderModels(const Vec3& eye) {
  std::vector<const Model*> transparentInstances;
  for (const std::pair<const std::string, Model>& model : sceneManager.scene.getObjects<Model>()) {
    const Model& instance = model.second;
    if (instance.name == "skybox") continue;

    if (instance.colour.w >= 1.0f) renderer.drawModel(meshManager, textureManager, instance);
    else                           transparentInstances.push_back(&instance);
  }

  for (size_t i = 0; i < transparentInstances.size(); ++i) {
    for (size_t j = 0; j < transparentInstances.size() - i - 1; ++j) {
      const Vec4& a = transparentInstances[j]->transform.pos;
      const Vec4& b = transparentInstances[j + 1]->transform.pos;

      float distA = (a.x - eye.x) * (a.x - eye.x) + (a.y - eye.y) * (a.y - eye.y) + (a.z - eye.z) * (a.z - eye.z);
      float distB = (b.x - eye.x) * (b.x - eye.x) + (b.y - eye.y) * (b.y - eye.y) + (b.z - eye.z) * (b.z - eye.z);

      if (distA < distB) {
        const Model* temp = transparentInstances[j];
        transparentInstances[j] = transparentInstances[j + 1];
        transparentInstances[j + 1] = temp;
      }
    }
  }

  for (const Model* instance : transparentInstances) renderer.drawModel(meshManager, textureManager, *instance);
}
void Engine::renderSkybox(const Vec3& eye) {
  Model* skybox{ nullptr };
  if (sceneManager.scene.getObjectByName(std::string("skybox"), skybox))
    renderer.drawSkybox(meshManager, textureManager, *skybox, eye);
}
