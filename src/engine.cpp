#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "StarletEngine/engine.hpp"
#include "StarletParsers/utils/log.hpp"

constexpr int default_width{ 1920 };
constexpr int default_height{ 1200 };

bool Engine::initialize(const unsigned int width, const unsigned int height, const char* title) {
  debugLog("Engine", "initialize", "Start time: " + std::to_string(glfwGetTime()), true);
  
  if (!windowManager.createWindow(width, height, title)) return false;
  glfwSetWindowUserPointer(windowManager.getWindow()->getGLFWwindow(), this);

  if (!setupShaders()) return false;

  setupGLState();
  return debugLog("Engine", "initialize", "Finish time: " + std::to_string(glfwGetTime()), true);
}

bool Engine::setupShaders() {
	debugLog("Engine", "setupShaders", "Start time: " + std::to_string(glfwGetTime()), true);

  shaderManager.setBasePath(assetPath);
  if (!shaderManager.createProgramFromPaths("shader1", "vertex_shader.glsl", "fragment_shader.glsl"))
    return error("Engine", "setupShaders", "Failed to create shader program from file");

  if (!renderer.setProgram(shaderManager.getProgramID("shader1")))
    return error("Engine", "setupShaders", "");
  
  return debugLog("Engine", "setupShaders", "Finish time: " + std::to_string(glfwGetTime()), true);
}
void Engine::setupGLState() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
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
bool Engine::loadSceneAssets() {
  if (sceneManager.scene.getScenePath().empty())
    if (!sceneManager.loadTxtScene(assetPath + "/scenes/Default.txt"))
      return error("Engine", "loadSceneMeshes", "No scene loaded and failed to load default scene");

  return loadSceneMeshes() && loadSceneLighting() && loadSceneTextures() && loadSceneTextureConnections();
}
bool Engine::loadSceneMeshes() {
  debugLog("Engine", "loadSceneMeshes", "Start time: " + std::to_string(glfwGetTime()), true);

  meshManager.setBasePath(assetPath);
  for (const std::pair<const std::string, Model>& model : sceneManager.scene.getObjects<Model>())
    if (!meshManager.addMesh(model.second.meshPath))
      return error("Engine", "loadSceneMeshes", "Failed to load mesh: " + model.second.meshPath);

  return debugLog("Engine", "loadSceneMeshes", "Finish time: " + std::to_string(glfwGetTime()), true);
}
bool Engine::loadSceneLighting() {
  debugLog("Engine", "loadSceneLighting", "Start time: " + std::to_string(glfwGetTime()), true);

  if (renderer.getProgram() == 0) return error("Engine", "loadSceneLighting", "No active shader program set before loading lighting");

  renderer.updateLightCount(sceneManager.scene.getObjectCount<Light>());
  renderer.updateLightUniforms(sceneManager.scene.getObjects<Light>());

  return debugLog("Engine", "loadSceneLighting", "Finish time: " + std::to_string(glfwGetTime()), true);
}
bool Engine::loadSceneTextures() {
  debugLog("Engine", "loadSceneTextures", "Start time: " + std::to_string(glfwGetTime()), true);

  textureManager.setBasePath(assetPath);
  for (const std::pair<const std::string, TextureData>& data : sceneManager.scene.getObjects<TextureData>()) {
    const TextureData& texture = data.second;
    
    if (!texture.isCube) {
      if(!textureManager.addTexture(texture.name, texture.faces[0]))
        return error("Engine", "loadSceneTextures", "Failed to load 2D texture: " + texture.name);
    }
    else if (!textureManager.addCubeTexture(texture.name, texture.faces))
      return error("Engine", "loadSceneTextures", "Failed to load cube map: " + texture.name);
  }

  return debugLog("Engine", "loadSceneTextures", "Finish time: " + std::to_string(glfwGetTime()), true);
}
bool Engine::loadSceneTextureConnections() {
  debugLog("Engine", "loadSceneTextureConnections", "Start time: " + std::to_string(glfwGetTime()), true);

  for (const std::pair<const std::string, TextureConnection>& data : sceneManager.scene.getObjects<TextureConnection>()) {
    const TextureConnection& connection = data.second;
    if (connection.slot >= Model::NUM_TEXTURES) return error("Engine", "loadSceneTextureConnection", "Slot out of range: " + std::to_string(connection.slot));
    
    Model* model;
    if (!sceneManager.scene.getObjectByName<Model>(connection.modelName, model))
      return error("Engine", "loadSceneTextureConnection", "Model " + connection.modelName + " not found for connection " + connection.name);

    if (connection.textureName.empty() || connection.mix <= 0.0f) {
      model->textureNames[connection.slot].clear();
      model->textureMixRatio[connection.slot] = 0.0f;

      bool any = false;
      for (unsigned int i = 0; i < Model::NUM_TEXTURES; ++i) {
        if (!model->textureNames[i].empty()) {
          any = true;
          break;
        }
      }

      model->useTextures = any;
      debugLog("Scene", "loadSceneTextureConnections", "unbind: " + connection.modelName + "[slot " + std::to_string(connection.slot) + "]", true);
      continue;
    }

    TextureData* texture;
    if(!sceneManager.scene.getObjectByName<TextureData>(connection.textureName, texture))
      return error("Engine", "loadSceneTextureConnection", "Texture " + connection.textureName + " not found for connection " + connection.name);

    model->useTextures = true;
    model->textureNames[connection.slot] = connection.textureName;
    model->textureMixRatio[connection.slot] = (connection.mix < 0.0f) ? 0.0f : (connection.mix > 1.0f ? 1.0f : connection.mix);
  }
  return debugLog("Engine", "loadSceneTextureConnections", "Finish time: " + std::to_string(glfwGetTime()), true);
}

void Engine::run() {
  windowManager.switchActiveWindowVisibility();

  while (!windowManager.getWindow()->shouldClose()) {	
    updateTime(static_cast<float>(glfwGetTime()));
    inputManager.update(windowManager.getWindow()->getGLFWwindow());

    Model* model{ nullptr };
    if (!sceneManager.scene.getObjectByIndex<Model>(modelController.current, model))
      error("Engine", "run", "No active model found for selected model");
    else modelController.update(*model, inputManager, deltaTime);

    renderFrame();
    windowManager.getWindow()->swapBuffers();
    windowManager.getWindow()->pollEvents();
  }
}

void Engine::renderFrame() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Camera* cam{ nullptr };
  if (!sceneManager.scene.getObjectByIndex<Camera>(cameraController.current, cam)) {
    error("Engine", "run", "No active camera found for selected camera");
    return;
  }

  renderer.updateCameraUniforms(cam->pos, Mat4::lookAt(cam->pos, cam->front), Mat4::perspective(cam->fov, windowManager.getWindow()->getAspect(), cam->nearPlane, cam->farPlane));
  cameraController.update(*cam, inputManager, deltaTime);
  renderer.updateLightUniforms(sceneManager.scene.getObjects<Light>());

  Model* skyBox{ nullptr };
  std::vector<const Model*> transparentInstances;
  for (std::pair<const std::string, Model>& model : sceneManager.scene.getObjects<Model>()) {
    Model& instance = model.second;
    if (instance.name == "skybox") {
      skyBox = &instance;
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

  if (skyBox) {
    skyBox->transform.pos = { cam->pos, 0.0f };
    renderer.bindSkyboxTexture(textureManager.getTextureID(skyBox->name));

    renderer.setModelIsSkybox(true);
    skyBox->isVisible = true;
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glDepthMask(GL_FALSE);
    renderer.drawModel(meshManager, textureManager, *skyBox);
    glDepthMask(GL_TRUE);
    glCullFace(GL_BACK);
    skyBox->isVisible = false;
    renderer.setModelIsSkybox(false);
  }

  glBindVertexArray(0);
}